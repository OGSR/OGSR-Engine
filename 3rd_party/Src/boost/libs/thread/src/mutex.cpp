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

#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/exceptions.hpp>
#include <boost/limits.hpp>
#include <stdexcept>
#include <cassert>
#include "timeconv.inl"

#if defined(BOOST_HAS_WINTHREADS)
#   include <windows.h>
#   include <time.h>
#elif defined(BOOST_HAS_PTHREADS)
#   include <errno.h>
#elif defined(BOOST_HAS_MPTASKS)
#   include <MacErrors.h>
#   include "mac/init.hpp"
#   include "mac/safe.hpp"
#endif

namespace boost {

#if defined(BOOST_HAS_WINTHREADS)
mutex::mutex() : m_mutex(0)
{
    try
    {
        m_mutex = reinterpret_cast<void*>(new CRITICAL_SECTION);
    }
    catch (...)
    {
    }
    if (!m_mutex)
        throw thread_resource_error();
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(m_mutex));
}

mutex::~mutex()
{
    DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(m_mutex));
    delete reinterpret_cast<LPCRITICAL_SECTION>(m_mutex);
}

void mutex::do_lock()
{
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(m_mutex));
}

void mutex::do_unlock()
{
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(m_mutex));
}

void mutex::do_lock(cv_state&)
{
    do_lock();
}

void mutex::do_unlock(cv_state&)
{
    do_unlock();
}

try_mutex::try_mutex()
{
    m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, 0));
    if (!m_mutex)
        throw thread_resource_error();
}

try_mutex::~try_mutex()
{
    int res = 0;
    res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void try_mutex::do_lock()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);
}

bool try_mutex::do_trylock()
{
    unsigned int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), 0);
    assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
    return res == WAIT_OBJECT_0;
}

void try_mutex::do_unlock()
{
    int res = 0;
    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void try_mutex::do_lock(cv_state&)
{
    do_lock();
}

void try_mutex::do_unlock(cv_state&)
{
    do_unlock();
}

timed_mutex::timed_mutex()
{
    m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, 0));
    if (!m_mutex)
        throw thread_resource_error();
}

timed_mutex::~timed_mutex()
{
    int res = 0;
    res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void timed_mutex::do_lock()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);
}

bool timed_mutex::do_trylock()
{
    unsigned int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), 0);
    assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
    return res == WAIT_OBJECT_0;
}

bool timed_mutex::do_timedlock(const xtime& xt)
{
    unsigned int res = 0;
    for (;;)
    {
        int milliseconds;
        to_duration(xt, milliseconds);

        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex),
            milliseconds);
        assert(res != WAIT_FAILED && res != WAIT_ABANDONED);

        if (res == WAIT_TIMEOUT)
        {
            xtime cur;
            xtime_get(&cur, TIME_UTC);
            if (xtime_cmp(xt, cur) > 0)
                continue;
        }

        return res == WAIT_OBJECT_0;
    }
}

void timed_mutex::do_unlock()
{
    int res = 0;
    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void timed_mutex::do_lock(cv_state&)
{
    do_lock();
}

void timed_mutex::do_unlock(cv_state&)
{
    do_unlock();
}
#elif defined(BOOST_HAS_PTHREADS)
mutex::mutex()
{
    int res = 0;
    res = pthread_mutex_init(&m_mutex, 0);
    if (res != 0)
        throw thread_resource_error();
}

mutex::~mutex()
{
    int res = 0;
    res = pthread_mutex_destroy(&m_mutex);
    assert(res == 0);
}

void mutex::do_lock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    if (res == EDEADLK) throw lock_error();
    assert(res == 0);
}

void mutex::do_unlock()
{
    int res = 0;
    res = pthread_mutex_unlock(&m_mutex);
    if (res == EPERM) throw lock_error();
    assert(res == 0);
}

void mutex::do_lock(cv_state&)
{
}

void mutex::do_unlock(cv_state& state)
{
    state.pmutex = &m_mutex;
}

try_mutex::try_mutex()
{
    int res = 0;
    res = pthread_mutex_init(&m_mutex, 0);
    if (res != 0)
        throw thread_resource_error();
}

try_mutex::~try_mutex()
{
    int res = 0;
    res = pthread_mutex_destroy(&m_mutex);
    assert(res == 0);
}

void try_mutex::do_lock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    if (res == EDEADLK) throw lock_error();
    assert(res == 0);
}

bool try_mutex::do_trylock()
{
    int res = 0;
    res = pthread_mutex_trylock(&m_mutex);
    if (res == EDEADLK) throw lock_error();
    assert(res == 0 || res == EBUSY);
    return res == 0;
}

void try_mutex::do_unlock()
{
    int res = 0;
    res = pthread_mutex_unlock(&m_mutex);
    if (res == EPERM) throw lock_error();
    assert(res == 0);
}

void try_mutex::do_lock(cv_state&)
{
}

void try_mutex::do_unlock(cv_state& state)
{
    state.pmutex = &m_mutex;
}

timed_mutex::timed_mutex()
    : m_locked(false)
{
    int res = 0;
    res = pthread_mutex_init(&m_mutex, 0);
    if (res != 0)
        throw thread_resource_error();

    res = pthread_cond_init(&m_condition, 0);
    if (res != 0)
    {
        pthread_mutex_destroy(&m_mutex);
        throw thread_resource_error();
    }
}

timed_mutex::~timed_mutex()
{
    assert(!m_locked);
    int res = 0;
    res = pthread_mutex_destroy(&m_mutex);
    assert(res == 0);

    res = pthread_cond_destroy(&m_condition);
    assert(res == 0);
}

void timed_mutex::do_lock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);

    while (m_locked)
    {
        res = pthread_cond_wait(&m_condition, &m_mutex);
        assert(res == 0);
    }

    assert(!m_locked);
    m_locked = true;

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
}

bool timed_mutex::do_trylock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);

    bool ret = false;
    if (!m_locked)
    {
        m_locked = true;
        ret = true;
    }

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
    return ret;
}

bool timed_mutex::do_timedlock(const xtime& xt)
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);

    timespec ts;
    to_timespec(xt, ts);

    while (m_locked)
    {
        res = pthread_cond_timedwait(&m_condition, &m_mutex, &ts);
        assert(res == 0 || res == ETIMEDOUT);

        if (res == ETIMEDOUT)
            break;
    }

    bool ret = false;
    if (!m_locked)
    {
        m_locked = true;
        ret = true;
    }

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
    return ret;
}

void timed_mutex::do_unlock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);

    assert(m_locked);
    m_locked = false;

    res = pthread_cond_signal(&m_condition);
    assert(res == 0);

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
}

void timed_mutex::do_lock(cv_state&)
{
    int res = 0;
    while (m_locked)
    {
        res = pthread_cond_wait(&m_condition, &m_mutex);
        assert(res == 0);
    }

    assert(!m_locked);
    m_locked = true;

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
}

void timed_mutex::do_unlock(cv_state& state)
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);

    assert(m_locked);
    m_locked = false;

    res = pthread_cond_signal(&m_condition);
    assert(res == 0);

    state.pmutex = &m_mutex;
}
#elif defined(BOOST_HAS_MPTASKS)

using threads::mac::detail::safe_enter_critical_region;

mutex::mutex()
{
}

mutex::~mutex()
{
}

void mutex::do_lock()
{
    OSStatus lStatus = noErr;
    lStatus = safe_enter_critical_region(m_mutex, kDurationForever,
        m_mutex_mutex);
    assert(lStatus == noErr);
}

void mutex::do_unlock()
{
    OSStatus lStatus = noErr;
    lStatus = MPExitCriticalRegion(m_mutex);
    assert(lStatus == noErr);
}

void mutex::do_lock(cv_state& /*state*/)
{
    do_lock();
}

void mutex::do_unlock(cv_state& /*state*/)
{
    do_unlock();
}

try_mutex::try_mutex()
{
}

try_mutex::~try_mutex()
{
}

void try_mutex::do_lock()
{
    OSStatus lStatus = noErr;
    lStatus = safe_enter_critical_region(m_mutex, kDurationForever,
        m_mutex_mutex);
    assert(lStatus == noErr);
}

bool try_mutex::do_trylock()
{
    OSStatus lStatus = noErr;
    lStatus = MPEnterCriticalRegion(m_mutex, kDurationImmediate);
    assert(lStatus == noErr || lStatus == kMPTimeoutErr);
    return lStatus == noErr;
}

void try_mutex::do_unlock()
{
    OSStatus lStatus = noErr;
    lStatus = MPExitCriticalRegion(m_mutex);
    assert(lStatus == noErr);
}

void try_mutex::do_lock(cv_state& /*state*/)
{
    do_lock();
}

void try_mutex::do_unlock(cv_state& /*state*/)
{
    do_unlock();
}

timed_mutex::timed_mutex()
{
}

timed_mutex::~timed_mutex()
{
}

void timed_mutex::do_lock()
{
    OSStatus lStatus = noErr;
    lStatus = safe_enter_critical_region(m_mutex, kDurationForever,
        m_mutex_mutex);
    assert(lStatus == noErr);
}

bool timed_mutex::do_trylock()
{
    OSStatus lStatus = noErr;
    lStatus = MPEnterCriticalRegion(m_mutex, kDurationImmediate);
    assert(lStatus == noErr || lStatus == kMPTimeoutErr);
    return(lStatus == noErr);
}

bool timed_mutex::do_timedlock(const xtime& xt)
{
    int microseconds;
    to_microduration(xt, microseconds);
    Duration lDuration = kDurationMicrosecond * microseconds;

    OSStatus lStatus = noErr;
    lStatus = safe_enter_critical_region(m_mutex, lDuration, m_mutex_mutex);
    assert(lStatus == noErr || lStatus == kMPTimeoutErr);

    return(lStatus == noErr);
}

void timed_mutex::do_unlock()
{
    OSStatus lStatus = noErr;
    lStatus = MPExitCriticalRegion(m_mutex);
    assert(lStatus == noErr);
}

void timed_mutex::do_lock(cv_state& /*state*/)
{
    do_lock();
}

void timed_mutex::do_unlock(cv_state& /*state*/)
{
    do_unlock();
}
#endif

} // namespace boost

// Change Log:
//   8 Feb 01  WEKEMPF Initial version.
