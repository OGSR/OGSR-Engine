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

#include <boost/config.hpp>

#if defined(BOOST_HAS_WINTHREADS)

#if defined(BOOST_THREAD_BUILD_DLL)

#include <boost/thread/detail/threadmon.hpp>

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#ifdef BOOST_MSVC
#   pragma warning(disable : 4786)
#endif

#include <list>
#include <set>
#include <algorithm>

typedef void (__cdecl * handler)(void);
typedef std::list<handler> exit_handlers;
typedef std::set<exit_handlers*> registered_handlers;

namespace
{
CRITICAL_SECTION cs;
DWORD key;
registered_handlers registry;
}

#if defined(__BORLANDC__)
#define DllMain DllEntryPoint
#endif

extern "C"
BOOL WINAPI DllMain(HANDLE /*module*/, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&cs);
        key = TlsAlloc();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
    {
        // Call the thread's exit handlers.
        exit_handlers* handlers =
            static_cast<exit_handlers*>(TlsGetValue(key));
        if (handlers)
        {
            for (exit_handlers::iterator it = handlers->begin();
                 it != handlers->end(); ++it)
            {
                (*it)();
            }

            // Remove the exit handler list from the registered lists
            // and then destroy it.
            EnterCriticalSection(&cs);
            registry.erase(handlers);
            LeaveCriticalSection(&cs);
            delete handlers;
        }
    }
    break;
    case DLL_PROCESS_DETACH:
    {
        // Assume the main thread is ending (call its handlers) and
        // all other threads have already ended.  If this DLL is
        // loaded and unloaded dynamically at run time
        // this is a bad assumption, but this is the best we can do.
        exit_handlers* handlers =
            static_cast<exit_handlers*>(TlsGetValue(key));
        if (handlers)
        {
            for (exit_handlers::iterator it = handlers->begin();
                 it != handlers->end(); ++it)
            {
                (*it)();
            }
        }

        // Destroy any remaining exit handlers.  Above we assumed
        // there'd only be the main thread left, but to insure we
        // don't get memory leaks we won't make that assumption
        // here.
        EnterCriticalSection(&cs);
        for (registered_handlers::iterator it = registry.begin();
             it != registry.end(); ++it)
        {
            delete (*it);
        }
        LeaveCriticalSection(&cs);
        DeleteCriticalSection(&cs);
        TlsFree(key);
    }
    break;
    }
    return TRUE;
}

extern "C" BOOST_THREAD_DECL int on_thread_exit(void (__cdecl * func)(void))
{
    // Get the exit handlers for the current thread, creating and registering
    // one if it doesn't exist.
    exit_handlers* handlers = static_cast<exit_handlers*>(TlsGetValue(key));
    if (!handlers)
    {
        try
        {
            handlers = new exit_handlers;
            // Handle broken implementations of operator new that don't throw.
            if (!handlers)
                return -1;
        }
        catch (...)
        {
            return -1;
        }

        // Attempt to set a TLS value for the new handlers.
        if (!TlsSetValue(key, handlers))
        {
            delete handlers;
            return -1;
        }

        // Attempt to register this new handler so that memory can be properly
        // cleaned up.
        try
        {
            EnterCriticalSection(&cs);
            registry.insert(handlers);
            LeaveCriticalSection(&cs);
        }
        catch (...)
        {
            LeaveCriticalSection(&cs);
            delete handlers;
            return -1;
        }
    }

    // Attempt to add the handler to the list of exit handlers. If it's been
    // previously added just report success and exit.
    try
    {
        handlers->push_front(func);
    }
    catch (...)
    {
        return -1;
    }

    return 0;
}

#endif // BOOST_THREAD_BUILD_DLL

#endif // BOOST_HAS_WINTHREADS
