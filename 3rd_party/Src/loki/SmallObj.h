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
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Nov 26, 2004

#ifndef SMALLOBJ_INC_
#define SMALLOBJ_INC_

#include "Threads.h"
#include "Singleton.h"
#include <cstddef>
#include <new> // needed for std::nothrow_t parameter.

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 256
#endif

#ifndef LOKI_DEFAULT_OBJECT_ALIGNMENT
#define LOKI_DEFAULT_OBJECT_ALIGNMENT 4
#endif


namespace Loki
{
    class FixedAllocator;

    class SmallObjAllocator
    {
    public:
        SmallObjAllocator( std::size_t pageSize, std::size_t maxObjectSize,
            std::size_t objectAlignSize );

        ~SmallObjAllocator( void );

        void * Allocate( std::size_t size, bool doThrow );

        void Deallocate( void * p, std::size_t size );

        inline std::size_t GetMaxObjectSize() const { return maxSmallObjectSize_; }

        inline std::size_t GetAlignment() const { return objectAlignSize_; }

    private:
        Loki::FixedAllocator * pool_;

        std::size_t maxSmallObjectSize_;

        std::size_t objectAlignSize_;
    };

////////////////////////////////////////////////////////////////////////////////
// class SmallObject
// Base class for polymorphic small objects, offers fast allocations &
//     deallocations.  Destructor is virtual and public.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel = DEFAULT_THREADING,
        std::size_t chunkSize = DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT
    >
    class SmallObject : public ThreadingModel<
        SmallObject< ThreadingModel, chunkSize, maxSmallObjectSize, objectAlignSize > >
    {

    	typedef ThreadingModel< SmallObject<ThreadingModel,
    			chunkSize, maxSmallObjectSize, objectAlignSize > > MyThreadingModel;

        struct MySmallObjAllocator : public SmallObjAllocator
        {
            MySmallObjAllocator()
            : SmallObjAllocator( chunkSize, maxSmallObjectSize, objectAlignSize )
            {}
        };
        // The typedef below would make things much simpler,
        //     but MWCW won't like it
        // typedef SingletonHolder<MySmallObjAllocator/*, CreateStatic,
        //        NoDestroy, ThreadingModel*/> MyAllocator;

    public:

#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0) && (LOKI_DEFAULT_OBJECT_ALIGNMENT != 0)

        /// Throwing single-object new.
        static void * operator new ( std::size_t size ) throw ( std::bad_alloc )
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return SingletonHolder< MySmallObjAllocator, CreateStatic,
                NoDestroy >::Instance().Allocate( size, true );
        }

        /// Non-throwing single-object new.
        static void * operator new ( std::size_t size, const std::nothrow_t & ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return SingletonHolder< MySmallObjAllocator, CreateStatic,
                NoDestroy >::Instance().Allocate( size, false );
        }

        /// Placement single-object new.
        static void * operator new ( std::size_t size, void * place )
        {
            return ::operator new( size, place );
        }

        /// Single-object delete.
        static void operator delete ( void * p, std::size_t size ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            SingletonHolder< MySmallObjAllocator, CreateStatic,
                NoDestroy >::Instance().Deallocate( p, size );
        }

        /// Non-throwing single-object delete.
        static void operator delete ( void * p, std::size_t size,
            const std::nothrow_t & ) throw()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            SingletonHolder< MySmallObjAllocator, CreateStatic,
                NoDestroy >::Instance().Deallocate( p, size );
        }

        /// Placement single-object delete.
        static void operator delete ( void * p, void * place )
        {
            ::operator delete ( p, place );
        }

#endif  // #if default template parameters are not zero

        virtual ~SmallObject() {}
    }; // end class SmallObject

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Nov. 26, 2004: re-implemented by Rich Sposato.
////////////////////////////////////////////////////////////////////////////////

#endif // SMALLOBJ_INC_

