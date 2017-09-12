////////////////////////////////////////////////////////////////////////////////
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

// Last update: Feb 24, 2003

#ifndef SINGLETON_INC_
#define SINGLETON_INC_

#include "Threads.h"
#include "MSVC6Helpers.h"		// for apply-template
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <new>

#if _MSC_VER <= 1200
#define VC_BROKEN_STD
#else
#define VC_BROKEN_STD std
#endif

namespace Loki
{
    namespace Private
    {
////////////////////////////////////////////////////////////////////////////////
// class LifetimeTracker
// Helper class for SetLongevity
////////////////////////////////////////////////////////////////////////////////

        class LifetimeTracker
        {
        public:
            LifetimeTracker(unsigned int x) : longevity_(x) 
            {}
            
            virtual ~LifetimeTracker() = 0;
            
            static bool Compare(const LifetimeTracker* lhs,
                const LifetimeTracker* rhs)
            {
                return rhs->longevity_ > lhs->longevity_;
            }
            
        private:
            unsigned int longevity_;
        };
        
        // Definition required
        inline LifetimeTracker::~LifetimeTracker() {} 
        
        // Helper data
        typedef LifetimeTracker** TrackerArray;
        extern TrackerArray pTrackerArray;
        extern unsigned int elements;

        // Helper destroyer function
        template <typename T>
        struct Deleter
        {
            static void Delete(T* pObj)
            { delete pObj; }
        };

        // Concrete lifetime tracker for objects of type T
        template <typename T, typename Destroyer>
        class ConcreteLifetimeTracker : public LifetimeTracker
        {
        public:
            ConcreteLifetimeTracker(T* p,unsigned int longevity, Destroyer d)
                : LifetimeTracker(longevity)
                , pTracked_(p)
                , destroyer_(d)
            {}
            
            ~ConcreteLifetimeTracker()
            { destroyer_(pTracked_); }
            
        private:
            T* pTracked_;
            Destroyer destroyer_;
        };

        void AtExitFn(); // declaration needed below
    
    } // namespace Private

////////////////////////////////////////////////////////////////////////////////
// function template SetLongevity
// Assigns an object a longevity; ensures ordered destructions of objects 
//     registered thusly during the exit sequence of the application
////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename Destroyer>
    void SetLongevity(T* pDynObject, unsigned int longevity,
        Destroyer d = Private::Deleter<T>::Delete)
    {
        using namespace Private;
        
        TrackerArray pNewArray = static_cast<TrackerArray>(
                VC_BROKEN_STD::realloc(pTrackerArray, sizeof(*pTrackerArray) * (elements + 1)));
        if (!pNewArray) throw std::bad_alloc();
        
        // Delayed assignment for exception safety
        pTrackerArray = pNewArray;

        LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(
            pDynObject, longevity, d);
                
        // Insert a pointer to the object into the queue
        TrackerArray pos = std::upper_bound(
            pTrackerArray, 
            pTrackerArray + elements, 
            p, 
            LifetimeTracker::Compare);
        std::copy_backward(
            pos, 
            pTrackerArray + elements,
            pTrackerArray + elements + 1);
        *pos = p;
        ++elements;
        
        // Register a call to AtExitFn
        VC_BROKEN_STD::atexit(Private::AtExitFn);
    }

////////////////////////////////////////////////////////////////////////////////
// class template CreateUsingNew
// Implementation of the CreationPolicy used by SingletonHolder
// Creates objects using a straight call to the new operator 
////////////////////////////////////////////////////////////////////////////////

    struct CreateUsingNew
	{
		template <class T>
		static T* Create(const volatile T* p = 0)
		{
			return new T;
		}
		template <class T>
		static void Destroy(T* p)
        { delete p; }
	};
    
////////////////////////////////////////////////////////////////////////////////
// class template CreateUsingNew
// Implementation of the CreationPolicy used by SingletonHolder
// Creates objects using a call to std::malloc, followed by a call to the 
//     placement new operator
////////////////////////////////////////////////////////////////////////////////

    struct CreateUsingMalloc
    {
		template <class T>
		static T* Create(const volatile T* dummy = 0)
        {
            void* p = VC_BROKEN_STD::malloc(sizeof(T));
            if (!p) return 0;
            return new(p) T;
        }
        
        template <class T> 
		static void Destroy(T* p)
        {
            p->~T();
            VC_BROKEN_STD::free(p);
        }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template CreateStatic
// Implementation of the CreationPolicy used by SingletonHolder
// Creates an object in static memory
// Implementation is slightly nonportable because it uses the MaxAlign trick 
//     (an union of all types to ensure proper memory alignment). This trick is 
//     nonportable in theory but highly portable in practice.
////////////////////////////////////////////////////////////////////////////////

    struct CreateStatic
    {
#ifdef _MSC_VER
#pragma warning( push ) 
 // alignment of a member was sensitive to packing
#pragma warning( disable : 4121 )
#endif // _MSC_VER        
		template <class T>
		union MaxAlign
        {
            char t_[sizeof(T)];
            short int shortInt_;
            int int_;
            long int longInt_;
            float float_;
            double double_;
            long double longDouble_;
            struct Test;
            int Test::* pMember_;
            int (Test::*pMemberFn_)(int);
        };
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER
        
        template <class T> 
		static T* Create(const volatile T* p = 0)
        {
            static MaxAlign<T> staticMemory_;
            return new(&staticMemory_) T;
        }
        
        template <class T> 
		static void Destroy(T* p)
        {
            p->~T();
        }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template DefaultLifetime
// Implementation of the LifetimePolicy used by SingletonHolder
// Schedules an object's destruction as per C++ rules
// Forwards to std::atexit
////////////////////////////////////////////////////////////////////////////////
	namespace
	{
		void Thrower(const char* s) {throw std::logic_error(s); }
	}
	
    struct DefaultLifetime
    {
        template <class T>
		static void ScheduleDestruction(T*, void (*pFun)())
        { VC_BROKEN_STD::atexit(pFun); }
        
		template <class T>
		static void OnDeadReference(const volatile T* p = 0 )
        { 
			// the throw will yield a C1001-internal compiler error.
			// The new level of indirection solves the problem
			// throw std::logic_error("Dead Reference Detected");
			Thrower("Dead Reference Detected");	
		}
    };

////////////////////////////////////////////////////////////////////////////////
// class template PhoenixSingleton
// Implementation of the LifetimePolicy used by SingletonHolder
// Schedules an object's destruction as per C++ rules, and it allows object 
//    recreation by not throwing an exception from OnDeadReference
////////////////////////////////////////////////////////////////////////////////

    
    class PhoenixSingleton
    {
	private:
		template <class T>
		struct StaticData
		{
			static bool destroyedOnce_;
		};
	public:
        template <class T>
		static void ScheduleDestruction(T*, void (*pFun)())
        {
			
#ifndef ATEXIT_FIXED
            if (!StaticData<T>::destroyedOnce_)
#endif
                VC_BROKEN_STD::atexit(pFun);
        }
        
        template <class T>
		static void OnDeadReference(const volatile T* p = 0 )
        {
#ifndef ATEXIT_FIXED
            StaticData<T>::destroyedOnce_ = true;
			
#endif
        }
        
    };
#ifndef ATEXIT_FIXED
	template <class T> 
	bool ::Loki::PhoenixSingleton::StaticData<T>::destroyedOnce_ = false;
#endif


        
////////////////////////////////////////////////////////////////////////////////
// class template Adapter
// Helper for SingletonWithLongevity below
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class T>
        struct Adapter
        {
            void operator()(T*) { pFun_(); return ; }
            void (*pFun_)();
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template SingletonWithLongevity
// Implementation of the LifetimePolicy used by SingletonHolder
// Schedules an object's destruction in order of their longevities
// Assumes a visible function GetLongevity(T*) that returns the longevity of the
//     object
////////////////////////////////////////////////////////////////////////////////
    
	class SingletonWithLongevity
    {
    public:
        template <class T>
		static void ScheduleDestruction(T* pObj, void (*pFun)())
        {
            Private::Adapter<T> adapter;
			adapter.pFun_ = pFun ;
            SetLongevity(pObj, GetLongevity(pObj), adapter);
        }
        
        template <class T>
		static void OnDeadReference(const volatile T* p = 0 )
        { 
			// the throw will yield a C1001-internal compiler error.
			// The new level of indirection solves the problem
			// throw std::logic_error("Dead Reference Detected");
			Thrower("Dead Reference Detected"); 
		}
    };

////////////////////////////////////////////////////////////////////////////////
// class template NoDestroy
// Implementation of the LifetimePolicy used by SingletonHolder
// Never destroys the object
////////////////////////////////////////////////////////////////////////////////

    
    struct NoDestroy
    {
        template <class T>
		static void ScheduleDestruction(T*, void (*)())
        {}
        
        template <class T>
		static void OnDeadReference(const volatile T* p = 0)
        {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template SingletonHolder
// Provides Singleton amenities for a type T
// To protect that type from spurious instantiations, you have to protect it
//     yourself.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class CreationPolicy = CreateUsingNew,
        class LifetimePolicy = DefaultLifetime,
        class ThreadingModel = SingleThreaded
    >
    class SingletonHolder
    {
    public:
        static T& Instance();
        
    private:
        // Helpers
        static void MakeInstance();
        static void DestroySingleton();
        
        // Protection
        SingletonHolder();
        
        // Data
        typedef typename Apply1<ThreadingModel, T*>::VolatileType VolatileType;
		static VolatileType pInstance_;
        static bool destroyed_;
    };
    
////////////////////////////////////////////////////////////////////////////////
// SingletonHolder's data
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        class C,
        class L,
        class M
    >
    typename SingletonHolder<T, C, L, M>::VolatileType
        SingletonHolder<T, C, L, M>::pInstance_;

    template
    <
        class T,
        class C,
        class L,
        class M
    >
    bool SingletonHolder<T, C, L, M>::destroyed_;

////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::Instance
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        class CreationPolicy,
        class LifetimePolicy,
        class ThreadingModel
    >
    inline T& SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel>::Instance()
    {
        if (!pInstance_)
        {
            MakeInstance();
        }
        return *pInstance_;
    }

////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::MakeInstance (helper for Instance)
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        class CreationPolicy,
        class LifetimePolicy,
        class ThreadingModel
    >
    void SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel>::MakeInstance()
    {
        //typename Apply1<ThreadingModel, T>::Lock guard;
		typename Apply1<ThreadingModel, SingletonHolder>::Lock guard;
        (void)guard;
        
        if (!pInstance_)
        {
            if (destroyed_)
            {
                LifetimePolicy::OnDeadReference(pInstance_);
                destroyed_ = false;
            }
            pInstance_ = CreationPolicy::Create(pInstance_);
            LifetimePolicy::ScheduleDestruction(pInstance_, 
                &DestroySingleton);
        }
    }

    template
    <
        class T,
        class CreationPolicy,
        class L,
        class M
    >
    void SingletonHolder<T, CreationPolicy, L, M>::DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy::Destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// May 21, 2001: Correct the volatile qualifier - credit due to Darin Adler
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Oct	06	2002: ported by Benjamin Kaufmann to MSVC 6.0
// Feb	24, 2003: changed parameter name of CreateUsingMalloc::Create, 
//					changed SingletonHolder::MakeInstance in accordance with 
//					Bug-report #691687 B.K.
////////////////////////////////////////////////////////////////////////////////

#endif // SINGLETON_INC_
