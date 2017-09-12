// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>
#include <cassert>

#if defined(BOOST_HAS_WINTHREADS)
#   include <windows.h>
#   include <process.h>
#elif defined(BOOST_HAS_MPTASKS)
#   include <DriverServices.h>

#   include "init.hpp"
#   include "safe.hpp"
#   include <boost/thread/tss.hpp>
#endif

#include "timeconv.inl"

namespace {

class thread_param
{
public:
    thread_param(const boost::function0<void>& threadfunc)
        : m_threadfunc(threadfunc), m_started(false)
    {
    }
    void wait()
    {
        boost::mutex::scoped_lock scoped_lock(m_mutex);
        while (!m_started)
            m_condition.wait(scoped_lock);
    }
    void started()
    {
        boost::mutex::scoped_lock scoped_lock(m_mutex);
        m_started = true;
        m_condition.notify_one();
    }

    boost::mutex m_mutex;
    boost::condition m_condition;
    const boost::function0<void>& m_threadfunc;
    bool m_started;
};

} // unnamed namespace

extern "C" {
#if defined(BOOST_HAS_WINTHREADS)
    unsigned __stdcall thread_proxy(void* param)
#elif defined(BOOST_HAS_PTHREADS)
        static void* thread_proxy(void* param)
#elif defined(BOOST_HAS_MPTASKS)
        static OSStatus thread_proxy(void* param)
#endif
    {
        try
        {
            thread_param* p = static_cast<thread_param*>(param);
            boost::function0<void> threadfunc = p->m_threadfunc;
            p->started();
            threadfunc();
        }
        catch (...)
        {
        }
#if defined(BOOST_HAS_MPTASKS)
        ::boost::detail::thread_cleanup();
#endif
        return 0;
    }

}

namespace boost {

thread::thread()
    : m_joinable(false)
{
#if defined(BOOST_HAS_WINTHREADS)
    m_thread = reinterpret_cast<void*>(GetCurrentThread());
    m_id = GetCurrentThreadId();
#elif defined(BOOST_HAS_PTHREADS)
    m_thread = pthread_self();
#elif defined(BOOST_HAS_MPTASKS)
    threads::mac::detail::thread_init();
    threads::mac::detail::create_singletons();
    m_pTaskID = MPCurrentTaskID();
    m_pJoinQueueID = kInvalidID;
#endif
}

thread::thread(const function0<void>& threadfunc)
    : m_joinable(true)
{
    thread_param param(threadfunc);
#if defined(BOOST_HAS_WINTHREADS)
    m_thread = reinterpret_cast<void*>(_beginthreadex(0, 0, &thread_proxy,
                                           &param, 0, &m_id));
    if (!m_thread)
        throw thread_resource_error();
#elif defined(BOOST_HAS_PTHREADS)
    int res = 0;
    res = pthread_create(&m_thread, 0, &thread_proxy, &param);
    if (res != 0)
        throw thread_resource_error();
#elif defined(BOOST_HAS_MPTASKS)
    threads::mac::detail::thread_init();
    threads::mac::detail::create_singletons();
    OSStatus lStatus = noErr;

    m_pJoinQueueID = kInvalidID;
    m_pTaskID = kInvalidID;

    lStatus = MPCreateQueue(&m_pJoinQueueID);
    if (lStatus != noErr) throw thread_resource_error();

    lStatus = MPCreateTask(&thread_proxy, &param, 0UL, m_pJoinQueueID, NULL,
        NULL, 0UL, &m_pTaskID);
    if (lStatus != noErr)
    {
        lStatus = MPDeleteQueue(m_pJoinQueueID);
        assert(lStatus == noErr);
        throw thread_resource_error();
    }
#endif
    param.wait();
}

thread::~thread()
{
    if (m_joinable)
    {
#if defined(BOOST_HAS_WINTHREADS)
        int res = 0;
        res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
        assert(res);
#elif defined(BOOST_HAS_PTHREADS)
        pthread_detach(m_thread);
#elif defined(BOOST_HAS_MPTASKS)
        assert(m_pJoinQueueID != kInvalidID);
        OSStatus lStatus = MPDeleteQueue(m_pJoinQueueID);
        assert(lStatus == noErr);
#endif
    }
}

bool thread::operator==(const thread& other) const
{
#if defined(BOOST_HAS_WINTHREADS)
    return other.m_id == m_id;
#elif defined(BOOST_HAS_PTHREADS)
    return pthread_equal(m_thread, other.m_thread) != 0;
#elif defined(BOOST_HAS_MPTASKS)
    return other.m_pTaskID == m_pTaskID;
#endif
}

bool thread::operator!=(const thread& other) const
{
    return !operator==(other);
}

void thread::join()
{
    int res = 0;
#if defined(BOOST_HAS_WINTHREADS)
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_thread), INFINITE);
    assert(res == WAIT_OBJECT_0);
    res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
    assert(res);
#elif defined(BOOST_HAS_PTHREADS)
    res = pthread_join(m_thread, 0);
    assert(res == 0);
#elif defined(BOOST_HAS_MPTASKS)
    OSStatus lStatus = threads::mac::detail::safe_wait_on_queue(
        m_pJoinQueueID, NULL, NULL, NULL, kDurationForever);
    assert(lStatus == noErr);
#endif
    // This isn't a race condition since any race that could occur would
    // have us in undefined behavior territory any way.
    m_joinable = false;
}

void thread::sleep(const xtime& xt)
{
    for (int foo=0; foo < 5; ++foo)
    {
#if defined(BOOST_HAS_WINTHREADS)
        int milliseconds;
        to_duration(xt, milliseconds);
        Sleep(milliseconds);
#elif defined(BOOST_HAS_PTHREADS)
#   if defined(BOOST_HAS_PTHREAD_DELAY_NP)
        timespec ts;
        to_timespec_duration(xt, ts);
        int res = 0;
        res = pthread_delay_np(&ts);
        assert(res == 0);
#   elif defined(BOOST_HAS_NANOSLEEP)
        timespec ts;
        to_timespec_duration(xt, ts);

        //  nanosleep takes a timespec that is an offset, not
        //  an absolute time.
        nanosleep(&ts, 0);
#   else
        mutex mx;
        mutex::scoped_lock lock(mx);
        condition cond;
        cond.timed_wait(lock, xt);
#   endif
#elif defined(BOOST_HAS_MPTASKS)
        int microseconds;
        to_microduration(xt, microseconds);
        Duration lMicroseconds(kDurationMicrosecond * microseconds);
        AbsoluteTime sWakeTime(DurationToAbsolute(lMicroseconds));
        threads::mac::detail::safe_delay_until(&sWakeTime);
#endif
        xtime cur;
        xtime_get(&cur, TIME_UTC);
        if (xtime_cmp(xt, cur) <= 0)
            return;
    }
}

void thread::yield()
{
#if defined(BOOST_HAS_WINTHREADS)
    Sleep(0);
#elif defined(BOOST_HAS_PTHREADS)
#   if defined(BOOST_HAS_SCHED_YIELD)
    int res = 0;
    res = sched_yield();
    assert(res == 0);
#   elif defined(BOOST_HAS_PTHREAD_YIELD)
    int res = 0;
    res = pthread_yield();
    assert(res == 0);
#   else
    xtime xt;
    xtime_get(&xt, TIME_UTC);
    sleep(xt);
#   endif
#elif defined(BOOST_HAS_MPTASKS)
    MPYield();
#endif
}

thread_group::thread_group()
{
}

thread_group::~thread_group()
{
    // We shouldn't have to scoped_lock here, since referencing this object
    // from another thread while we're deleting it in the current thread is
    // going to lead to undefined behavior any way.
    for (std::list<thread*>::iterator it = m_threads.begin();
         it != m_threads.end(); ++it)
    {
        delete (*it);
    }
}

thread* thread_group::create_thread(const function0<void>& threadfunc)
{
    // No scoped_lock required here since the only "shared data" that's
    // modified here occurs inside add_thread which does scoped_lock.
    std::auto_ptr<thread> thrd(new thread(threadfunc));
    add_thread(thrd.get());
    return thrd.release();
}

void thread_group::add_thread(thread* thrd)
{
    mutex::scoped_lock scoped_lock(m_mutex);

    // For now we'll simply ignore requests to add a thread object multiple
    // times. Should we consider this an error and either throw or return an
    // error value?
    std::list<thread*>::iterator it = std::find(m_threads.begin(),
        m_threads.end(), thrd);
    assert(it == m_threads.end());
    if (it == m_threads.end())
        m_threads.push_back(thrd);
}

void thread_group::remove_thread(thread* thrd)
{
    mutex::scoped_lock scoped_lock(m_mutex);

    // For now we'll simply ignore requests to remove a thread object that's
    // not in the group. Should we consider this an error and either throw or
    // return an error value?
    std::list<thread*>::iterator it = std::find(m_threads.begin(),
        m_threads.end(), thrd);
    assert(it != m_threads.end());
    if (it != m_threads.end())
        m_threads.erase(it);
}

void thread_group::join_all()
{
    mutex::scoped_lock scoped_lock(m_mutex);
    for (std::list<thread*>::iterator it = m_threads.begin();
         it != m_threads.end(); ++it)
    {
        (*it)->join();
    }
}

} // namespace boost

// Change Log:
//    8 Feb 01  WEKEMPF Initial version.
//    1 Jun 01  WEKEMPF Added boost::thread initial implementation.
//    3 Jul 01  WEKEMPF Redesigned boost::thread to be noncopyable.
