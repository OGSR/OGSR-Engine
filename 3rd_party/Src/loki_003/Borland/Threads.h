///////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: August 9, 2002

#ifndef THREADS_INC_
#define THREADS_INC_

////////////////////////////////////////////////////////////////////////////////
// macro DEFAULT_THREADING
// Selects the default threading model for certain components of Loki
// If you don't define it, it defaults to single-threaded
// All classes in Loki have configurable threading model; DEFAULT_THREADING
// affects only default template arguments
////////////////////////////////////////////////////////////////////////////////

#ifdef __WIN32__
#  ifndef _WINDOWS_
#    define WIN32_LEAN_AND_MEAN
#      include <windows.h>
#  endif
#endif
#include <cassert>


// Last update: June 20, 2001

#ifndef DEFAULT_THREADING
#define DEFAULT_THREADING /**/ ::Loki::SingleThreaded
#endif

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
// Implementation of the ThreadingModel policy used by various classes
// Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////

    template <class Host>
    class SingleThreaded
    {
    public:
        struct Lock
        {
            Lock() {}
            Lock(const Host&) {}
        };
        
        typedef Host VolatileType;

        typedef int IntType; 

        static IntType AtomicAdd(volatile IntType& lval, IntType val)
        { return lval += val; }
        
        static IntType AtomicSubtract(volatile IntType& lval, IntType val)
        { return lval -= val; }

        static IntType AtomicMultiply(volatile IntType& lval, IntType val)
        { return lval *= val; }
        
        static IntType AtomicDivide(volatile IntType& lval, IntType val)
        { return lval /= val; }
        
        static IntType AtomicIncrement(volatile IntType& lval)
        { return ++lval; }
                
        static void AtomicAssign(volatile IntType & lval, IntType val)
        { lval = val; }
        
        static void AtomicAssign(IntType & lval, volatile IntType & val)
        { lval = val; }
    };
    
//#ifdef _WINDOWS_  Borland doesn't use this
#ifdef __WIN32__

////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////

    template <class Host>
    class ObjectLevelLockable
    {
        CRITICAL_SECTION mtx_;

    public:
        ObjectLevelLockable()
        {
            ::InitializeCriticalSection(&mtx_);
        }

        ~ObjectLevelLockable()
        {
            ::DeleteCriticalSection(&mtx_);
        }

        class Lock;
        friend class Lock;
        
        class Lock
        {
            ObjectLevelLockable& host_;

            Lock(const Lock&);
            Lock& operator=(const Lock&);
        public:
            Lock(Host& host) : host_(host)
            {
                ::EnterCriticalSection(&host_.mtx_);
            }
            ~Lock()
            {
                ::LeaveCriticalSection(&host_.mtx_);
            }
        };

        typedef volatile Host VolatileType;

        typedef LONG IntType; 

        static IntType AtomicIncrement(volatile IntType& lval)
        { return InterlockedIncrement(&const_cast<IntType&>(lval)); }
                
        static void AtomicAssign(volatile IntType& lval, IntType val)
        { InterlockedExchange(&const_cast<IntType&>(lval), val); }

        static void AtomicAssign(IntType& lval, volatile IntType& val)
        { InterlockedExchange(&lval, val); }
    };

    template <class Host>
    class ClassLevelLockable
    {
        static CRITICAL_SECTION mtx_;
        static volatile LONG mtx_initialisation_started;
        static volatile LONG mtx_initialisation_done;

        //### Borland C++ doesn't like this (it is ignored)
        /*
        struct Initializer;
        friend struct Initializer;
        struct Initializer
        {
            Initializer()
            {
                ::InitializeCriticalSection(&mtx_);
            }
            ~Initializer()
            {
                ::DeleteCriticalSection(&mtx_);
            }
        };

        static Initializer initializer_;
        */

    public:
        class Lock;
        friend class Lock;

        typedef volatile Host VolatileType;

        typedef LONG IntType;

        static IntType AtomicIncrement(volatile IntType& lval)
        { return InterlockedIncrement(&const_cast<IntType&>(lval)); }

        static void AtomicAssign(volatile IntType& lval, IntType val)
        { InterlockedExchange(&const_cast<IntType&>(lval), val); }

        static void AtomicAssign(IntType& lval, volatile IntType& val)
        { InterlockedExchange(&lval, val); }

private:
        // used to protect critical section initialisation
        static LONG AtomicExchange(volatile IntType& lval, IntType& new_value) {
            return InterlockedExchange(&lval, new_value);
        }
public:

        class Lock
        {
            Lock(const Lock&);
            Lock& operator=(const Lock&);

            //### BCB: the initialisation itself must be protected as it is not done during static initialisation
            void initialize_impl(void)
            {
                LONG now = true;
                now = ClassLevelLockable<Host>::AtomicExchange(ClassLevelLockable<Host>::mtx_initialisation_started, now);
                assert(ClassLevelLockable<Host>::mtx_initialisation_started);
                if (!now) {
                    assert(!ClassLevelLockable<Host>::mtx_initialisation_done);
                    ::InitializeCriticalSection(&mtx_);
                    ClassLevelLockable<Host>::mtx_initialisation_done = true;
                } else {
                    // critical section is just being initialized by other thread
                    while (!ClassLevelLockable<Host>::mtx_initialisation_done) {
                        Sleep(0);
                    }
                }
            }

        public:
            Lock()
            {
                //### Here's the trick to make it working on BC++B 6.0
                // (because the static Initializer struct is ignored)
                //
                // The critical section isn't deleted by atexit().
                //    Shouldn't matter in practise. You may add it here if you want.
                if (!mtx_initialisation_done) {
                    initialize_impl();
                }
                ::EnterCriticalSection(&mtx_);
            }
            Lock(Host&)
            {
                if (!mtx_initialisation_done) {
                    initialize_impl();
                }
                ::EnterCriticalSection(&mtx_);
            }
            ~Lock()
            {
                ::LeaveCriticalSection(&mtx_);
            }
        };
    };

    template <class Host>
    CRITICAL_SECTION ClassLevelLockable<Host>::mtx_;

    template <class Host>
    LONG volatile ClassLevelLockable<Host>::mtx_initialisation_started;
    template <class Host>
    LONG volatile ClassLevelLockable<Host>::mtx_initialisation_done;

//### Borland C++ does like this
//    template <class Host>
//    typename ClassLevelLockable<Host>::Initializer
//    ClassLevelLockable<Host>::initializer_;

#endif
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June    20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// January 10, 2002: Fixed bug in AtomicDivide - credit due to Jordi Guerrero
// July    16, 2002: Ported by Terje Slettebø and Pavel Vozenilek to BCC 5.6
// March   06, 2003: Removed wrong AtomicDivide - credit due to Arthur Pawlak
////////////////////////////////////////////////////////////////////////////////

#endif
