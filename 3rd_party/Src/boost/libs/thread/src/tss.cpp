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

#include <boost/thread/tss.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/exceptions.hpp>
#include <stdexcept>
#include <cassert>

#if defined(BOOST_HAS_WINTHREADS)
#   include <windows.h>
#endif

#if defined(BOOST_HAS_WINTHREADS)
#include <boost/thread/detail/threadmon.hpp>
#include <map>
namespace {
typedef std::pair<void(*)(void*), void*> cleanup_info;
typedef std::map<int, cleanup_info> cleanup_handlers;

DWORD key;
boost::once_flag once = BOOST_ONCE_INIT;

void init_cleanup_key()
{
    key = TlsAlloc();
    assert(key != 0xFFFFFFFF);
}

void __cdecl cleanup()
{
    cleanup_handlers* handlers = static_cast<cleanup_handlers*>(
        TlsGetValue(key));
    for (cleanup_handlers::iterator it = handlers->begin();
         it != handlers->end(); ++it)
    {
        cleanup_info info = it->second;
        if (info.second)
            info.first(info.second);
    }
    delete handlers;
}

cleanup_handlers* get_handlers()
{
    boost::call_once(&init_cleanup_key, once);

    cleanup_handlers* handlers = static_cast<cleanup_handlers*>(
        TlsGetValue(key));
    if (!handlers)
    {
        try
        {
            handlers = new cleanup_handlers;
        }
        catch (...)
        {
            return 0;
        }
        int res = 0;
        res = TlsSetValue(key, handlers);
        assert(res);
        res = on_thread_exit(&cleanup);
        assert(res == 0);
    }

    return handlers;
}
}
#elif defined(BOOST_HAS_MPTASKS)
#include <map>
namespace {
typedef std::pair<void(*)(void*), void*> cleanup_info;
typedef std::map<int, cleanup_info> cleanup_handlers;

TaskStorageIndex key;
boost::once_flag once = BOOST_ONCE_INIT;

void init_cleanup_key()
{
    OSStatus lStatus = MPAllocateTaskStorageIndex(&key);
    assert(lStatus == noErr);
}

cleanup_handlers* get_handlers()
{
    boost::call_once(&init_cleanup_key, once);

    cleanup_handlers* handlers = reinterpret_cast<cleanup_handlers*>(
        MPGetTaskStorageValue(key));
    if (!handlers)
    {
        try
        {
            handlers = new cleanup_handlers;
        }
        catch (...)
        {
            return 0;
        }
        OSStatus lStatus = noErr;
        lStatus = MPSetTaskStorageValue(key,
            reinterpret_cast<TaskStorageValue>(handlers));
        assert(lStatus == noErr);
        // TODO - create a generalized mechanism for registering thread exit
        // functions and use it here.
    }

    return handlers;
}

}

namespace boost {

namespace detail {


void thread_cleanup()
{
    cleanup_handlers* handlers = reinterpret_cast<cleanup_handlers*>(
        MPGetTaskStorageValue(key));
    if(handlers != NULL)
    {
        for (cleanup_handlers::iterator it = handlers->begin();
             it != handlers->end(); ++it)
        {
            cleanup_info info = it->second;
            if (info.second)
                info.first(info.second);
        }
        delete handlers;
    }
}


} // namespace detail

} // namespace boost

#endif

namespace boost { namespace detail {

#if defined(BOOST_HAS_WINTHREADS)
tss::tss(void (*cleanup)(void*))
{
    m_key = TlsAlloc();
    if (m_key == 0xFFFFFFFF)
        throw thread_resource_error();

    m_cleanup = cleanup;
}

tss::~tss()
{
    int res = 0;
    res = TlsFree(m_key);
    assert(res);
}

void* tss::get() const
{
    return TlsGetValue(m_key);
}

bool tss::set(void* value)
{
    if (value && m_cleanup)
    {
        cleanup_handlers* handlers = get_handlers();
        assert(handlers);
        if (!handlers)
            return false;
        cleanup_info info(m_cleanup, value);
        (*handlers)[m_key] = info;
    }
    return !!TlsSetValue(m_key, value);
}
#elif defined(BOOST_HAS_PTHREADS)
tss::tss(void (*cleanup)(void*))
{
    int res = 0;
    res = pthread_key_create(&m_key, cleanup);
    if (res != 0)
        throw thread_resource_error();
}

tss::~tss()
{
    int res = 0;
    res = pthread_key_delete(m_key);
    assert(res == 0);
}

void* tss::get() const
{
    return pthread_getspecific(m_key);
}

bool tss::set(void* value)
{
    return pthread_setspecific(m_key, value) == 0;
}
#elif defined(BOOST_HAS_MPTASKS)
tss::tss(void (*cleanup)(void*))
{
    OSStatus lStatus = MPAllocateTaskStorageIndex(&m_key);
    if(lStatus != noErr)
        throw thread_resource_error();

    m_cleanup = cleanup;
}

tss::~tss()
{
    OSStatus lStatus = MPDeallocateTaskStorageIndex(m_key);
    assert(lStatus == noErr);
}

void* tss::get() const
{
    TaskStorageValue ulValue = MPGetTaskStorageValue(m_key);
    return(reinterpret_cast<void *>(ulValue));
}

bool tss::set(void* value)
{
    if (value && m_cleanup)
    {
        cleanup_handlers* handlers = get_handlers();
        assert(handlers);
        if (!handlers)
            return false;
        cleanup_info info(m_cleanup, value);
        (*handlers)[m_key] = info;
    }
    OSStatus lStatus = MPSetTaskStorageValue(m_key,
        reinterpret_cast<TaskStorageValue>(value));
    return(lStatus == noErr);
}
#endif

} // namespace detail
} // namespace boost

// Change Log:
//   6 Jun 01  WEKEMPF Initial version.
