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


#include "SmallObj.h"

#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>


using namespace Loki;


namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class FixedAllocator
// Offers services for allocating fixed-sized objects
////////////////////////////////////////////////////////////////////////////////

    class FixedAllocator
    {
    private:
        struct Chunk
        {
            bool Init( std::size_t blockSize, unsigned char blocks );
            void* Allocate(std::size_t blockSize);
            void Deallocate(void* p, std::size_t blockSize);
            void Reset(std::size_t blockSize, unsigned char blocks);
            void Release();
            inline bool HasBlock( unsigned char * p, std::size_t chunkLength ) const
            { return ( pData_ <= p ) && ( p < pData_ + chunkLength ); }

            inline bool HasAvailable( unsigned char numBlocks ) const
            { return ( blocksAvailable_ == numBlocks ); }

            inline bool IsFilled( void ) const
            { return ( 0 == blocksAvailable_ ); }

            unsigned char* pData_;
            unsigned char
                firstAvailableBlock_,
                blocksAvailable_;
        };

        // Internal functions
        void DoDeallocate(void* p);

		bool MakeNewChunk( void );

        Chunk * VicinityFind( void * p );

        /// Not implemented.
        FixedAllocator(const FixedAllocator&);
        /// Not implemented.
        FixedAllocator& operator=(const FixedAllocator&);

        // Data
        std::size_t blockSize_;
        unsigned char numBlocks_;
        typedef std::vector<Chunk> Chunks;
        typedef Chunks::iterator ChunkIter;
        typedef Chunks::const_iterator ChunkCIter;
        Chunks chunks_;
        Chunk* allocChunk_;
        Chunk* deallocChunk_;
        Chunk * emptyChunk_;

    public:
        // Create a FixedAllocator able to manage blocks of 'blockSize' size
        FixedAllocator();
        ~FixedAllocator();

        void Initialize( std::size_t blockSize, std::size_t pageSize );

        // Allocate a memory block
        void * Allocate( void );

        // Deallocate a memory block previously allocated with Allocate()
        // (if that's not the case, the behavior is undefined)
        bool Deallocate( void * p, bool doChecks );
        // Returns the block size with which the FixedAllocator was initialized
        inline std::size_t BlockSize() const
        { return blockSize_; }

    };


////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Chunk::Init
// Initializes a chunk object
////////////////////////////////////////////////////////////////////////////////

bool FixedAllocator::Chunk::Init( std::size_t blockSize, unsigned char blocks )
{
    assert(blockSize > 0);
    assert(blocks > 0);
    // Overflow check
    const std::size_t allocSize = blockSize * blocks;
    assert( allocSize / blockSize == blocks);

#ifdef USE_NEW_TO_ALLOCATE
	// If this new operator fails, it will throw, and the exception will get
	// caught one layer up.
    pData_ = new unsigned char[ allocSize ];
#else
	// malloc can't throw, so its only way to indicate an error is to return
	// a NULL pointer, so we have to check for that.
    pData_ = static_cast< unsigned char * >( ::malloc( allocSize ) );
    if ( NULL == pData_ ) return false;
#endif

    Reset( blockSize, blocks );
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Chunk::Reset
// Clears an already allocated chunk
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::Chunk::Reset(std::size_t blockSize, unsigned char blocks)
{
    assert(blockSize > 0);
    assert(blocks > 0);
    // Overflow check
    assert((blockSize * blocks) / blockSize == blocks);

    firstAvailableBlock_ = 0;
    blocksAvailable_ = blocks;

    unsigned char i = 0;
    unsigned char* p = pData_;
    for (; i != blocks; p += blockSize)
    {
        *p = ++i;
    }
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Chunk::Release
// Releases the data managed by a chunk
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::Chunk::Release()
{
	assert( NULL != pData_ );
#ifdef USE_NEW_TO_ALLOCATE
    delete [] pData_;
#else
    ::free( static_cast< void * >( pData_ ) );
#endif
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Chunk::Allocate
// Allocates a block from a chunk
////////////////////////////////////////////////////////////////////////////////

void* FixedAllocator::Chunk::Allocate(std::size_t blockSize)
{
    if ( IsFilled() ) return NULL;

    assert((firstAvailableBlock_ * blockSize) / blockSize == 
        firstAvailableBlock_);
    unsigned char * pResult = pData_ + (firstAvailableBlock_ * blockSize);
    firstAvailableBlock_ = *pResult;
    --blocksAvailable_;

    return pResult;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Chunk::Deallocate
// Dellocates a block from a chunk
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::Chunk::Deallocate(void* p, std::size_t blockSize)
{
    assert(p >= pData_);

    unsigned char* toRelease = static_cast<unsigned char*>(p);
    // Alignment check
    assert((toRelease - pData_) % blockSize == 0);

    *toRelease = firstAvailableBlock_;
    firstAvailableBlock_ = static_cast<unsigned char>(
        (toRelease - pData_) / blockSize);
    // Truncation check
    assert(firstAvailableBlock_ == (toRelease - pData_) / blockSize);

    ++blocksAvailable_;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::FixedAllocator
// Creates a FixedAllocator object of a fixed block size
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::FixedAllocator()
    : blockSize_( 0 )
    , allocChunk_( NULL )
    , deallocChunk_( NULL )
    , emptyChunk_( NULL )
{
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::~FixedAllocator
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::~FixedAllocator()
{
    for ( ChunkIter i( chunks_.begin() ); i != chunks_.end(); ++i )
       i->Release();
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Initialize
// Initializes the operational constraints for the FixedAllocator
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::Initialize( std::size_t blockSize, std::size_t pageSize )
{
    assert( blockSize > 0 );
    assert( pageSize >= blockSize );
    blockSize_ = blockSize;

    std::size_t numBlocks = pageSize / blockSize;
    if (numBlocks > UCHAR_MAX) numBlocks = UCHAR_MAX;
    else if (numBlocks == 0) numBlocks = 8 * blockSize;

    numBlocks_ = static_cast<unsigned char>(numBlocks);
    assert(numBlocks_ == numBlocks);
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::MakeNewChunk
// Allocates a new Chunk for a FixedAllocator.
////////////////////////////////////////////////////////////////////////////////

bool FixedAllocator::MakeNewChunk( void )
{
	bool allocated = false;
	try
	{
		// Calling chunks_.reserve *before* creating and initializing the new
		// Chunk means that nothing is leaked by this function in case an
		// exception is thrown from reserve.
		chunks_.reserve( chunks_.size() + 1 );
		Chunk newChunk;
		allocated = newChunk.Init( blockSize_, numBlocks_ );
		if ( allocated )
			chunks_.push_back( newChunk );
	}
	catch ( ... )
	{
		allocated = false;
	}
	if ( !allocated ) return false;

    allocChunk_ = &chunks_.back();
    deallocChunk_ = &chunks_.front();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Allocate
// Allocates a block of fixed size
////////////////////////////////////////////////////////////////////////////////

void * FixedAllocator::Allocate( void )
{
    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    assert( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );

    if ( ( NULL == allocChunk_ ) || allocChunk_->IsFilled() )
    {
        if ( NULL != emptyChunk_ )
        {
            allocChunk_ = emptyChunk_;
            emptyChunk_ = NULL;
        }
        else
        {
            for ( ChunkIter i( chunks_.begin() ); ; ++i )
            {
                if ( chunks_.end() == i )
                {
					if ( !MakeNewChunk() )
						return NULL;
                    break;
                }
                if ( !i->IsFilled() )
                {
                    allocChunk_ = &*i;
                    break;
                }
            }
        }
    }

    assert( allocChunk_ != NULL );
    assert( !allocChunk_->IsFilled() );
    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    assert( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );

    return allocChunk_->Allocate(blockSize_);
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::Deallocate
// Deallocates a block previously allocated with Allocate
// (undefined behavior if called with the wrong pointer)
////////////////////////////////////////////////////////////////////////////////

bool FixedAllocator::Deallocate( void * p, bool doChecks )
{
    if ( doChecks )
    {
        assert(!chunks_.empty());
        assert(&chunks_.front() <= deallocChunk_);
        assert(&chunks_.back() >= deallocChunk_);
        assert( &chunks_.front() <= allocChunk_ );
        assert( &chunks_.back() >= allocChunk_ );
    }

    Chunk * foundChunk = VicinityFind( p );
    if ( doChecks )
    {
        assert( NULL != foundChunk );
    }
    else if ( NULL == foundChunk )
        return false;

    deallocChunk_ = foundChunk;
    DoDeallocate(p);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::VicinityFind (internal)
// Finds the chunk corresponding to a pointer, using an efficient search
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::Chunk * FixedAllocator::VicinityFind( void * p )
{
    if ( chunks_.empty() ) return NULL;
    assert(deallocChunk_);

    unsigned char * pc = static_cast< unsigned char * >( p );
    const std::size_t chunkLength = numBlocks_ * blockSize_;

    Chunk* lo = deallocChunk_;
    Chunk* hi = deallocChunk_ + 1;
    Chunk* loBound = &chunks_.front();
    Chunk* hiBound = &chunks_.back() + 1;
    
    // Special case: deallocChunk_ is the last in the array
    if (hi == hiBound) hi = NULL;

    for (;;)
    {
        if (lo)
        {
            if ( lo->HasBlock( pc, chunkLength ) ) return lo;
            if ( lo == loBound )
			{
				lo = NULL;
				if ( NULL == hi ) break;
			}
            else --lo;
        }

        if (hi)
        {
            if ( hi->HasBlock( pc, chunkLength ) ) return hi;
            if ( ++hi == hiBound )
			{
				hi = NULL;
				if ( NULL == lo ) break;
			}
        }
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::DoDeallocate (internal)
// Performs deallocation. Assumes deallocChunk_ points to the correct chunk
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::DoDeallocate(void* p)
{
    assert( deallocChunk_->HasBlock( static_cast< unsigned char * >( p ),
        numBlocks_ * blockSize_ ) );
    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    assert( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );

    // call into the chunk, will adjust the inner list but won't release memory
    deallocChunk_->Deallocate(p, blockSize_);

    if ( deallocChunk_->HasAvailable( numBlocks_ ) )
    {
        assert( emptyChunk_ != deallocChunk_ );
        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( NULL != emptyChunk_ )
        {
            // If last Chunk is empty, just change what deallocChunk_
            // points to, and release the last.  Otherwise, swap an empty
            // Chunk with the last, and then release it.
            Chunk * lastChunk = &chunks_.back();
            if ( lastChunk == deallocChunk_ )
                deallocChunk_ = emptyChunk_;
            else if ( lastChunk != emptyChunk_ )
                std::swap( *emptyChunk_, *lastChunk );
            assert( lastChunk->HasAvailable( numBlocks_ ) );
            lastChunk->Release();
            chunks_.pop_back();
            allocChunk_ = deallocChunk_;
        }
        emptyChunk_ = deallocChunk_;
    }

    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    assert( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );
}

////////////////////////////////////////////////////////////////////////////////
// GetOffset
// Calculates index into array where a FixedAllocator of numBytes is located.
////////////////////////////////////////////////////////////////////////////////

inline std::size_t GetOffset( std::size_t numBytes, std::size_t alignment )
{
    const std::size_t alignExtra = alignment-1;
    return ( numBytes + alignExtra ) / alignment;
}

////////////////////////////////////////////////////////////////////////////////
// DefaultAllocator
// Call to default allocator when SmallObjAllocator decides not to handle request.
////////////////////////////////////////////////////////////////////////////////

void * DefaultAllocator( std::size_t numBytes, bool doThrow )
{
#ifdef USE_NEW_TO_ALLOCATE
    return doThrow ? ::operator new( numBytes ) :
        ::operator new( numBytes, std::nothrow_t() );
#else
    void * p = ::malloc( numBytes );
    if ( doThrow && ( NULL == p ) )
        throw std::bad_alloc();
    return p;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// DefaultDeallocator
// Call to default deallocator when SmallObjAllocator decides not to handle request.
////////////////////////////////////////////////////////////////////////////////

void DefaultDeallocator( void * p )
{
#ifdef USE_NEW_TO_ALLOCATE
    ::operator delete( p );
#else
    ::free( p );
#endif
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::SmallObjAllocator
// Creates a SmallObjAllocator, and all the FixedAllocators within it.  Each
// FixedAllocator is then initialized to use the correct Chunk size.
////////////////////////////////////////////////////////////////////////////////

SmallObjAllocator::SmallObjAllocator( std::size_t pageSize,
    std::size_t maxObjectSize, std::size_t objectAlignSize ) :
    pool_( NULL ),
    maxSmallObjectSize_( maxObjectSize ),
    objectAlignSize_( objectAlignSize )
{
    assert( 0 != objectAlignSize );
    const std::size_t allocCount = GetOffset( maxObjectSize, objectAlignSize );
    pool_ = new FixedAllocator[ allocCount ];
    for ( std::size_t i = 0; i < allocCount; ++i )
        pool_[ i ].Initialize( ( i+1 ) * objectAlignSize, pageSize );
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::~SmallObjAllocator
// Deletes all memory consumed by SmallObjAllocator.
// This deletes all the FixedAllocator's in the pool.
////////////////////////////////////////////////////////////////////////////////

SmallObjAllocator::~SmallObjAllocator( void )
{
    delete [] pool_;
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::Allocate
// Handles request to allocate numBytes for 1 object.
// This acts in constant-time - except for the calls to DefaultAllocator
// and sometimes FixedAllocator::Allocate.  It throws bad_alloc only if the
// doThrow parameter is true and can't allocate another block.  Otherwise, it
// provides the no-throw exception safety level.
////////////////////////////////////////////////////////////////////////////////

void * SmallObjAllocator::Allocate( std::size_t numBytes, bool doThrow )
{
    if ( numBytes > GetMaxObjectSize() )
        return DefaultAllocator( numBytes, doThrow );

    assert( NULL != pool_ );
    if ( 0 == numBytes ) numBytes = 1;
    const std::size_t index = GetOffset( numBytes, GetAlignment() ) - 1;
    const std::size_t allocCount = GetOffset( GetMaxObjectSize(), GetAlignment() );
    assert( index < allocCount );

    FixedAllocator & allocator = pool_[ index ];
    assert( allocator.BlockSize() >= numBytes );
    assert( allocator.BlockSize() < numBytes + GetAlignment() );
    void * place = allocator.Allocate();
	if ( ( NULL == place ) && doThrow )
    {
#if _MSC_VER
        throw std::bad_alloc( "could not allocate small object" );
#else
        // GCC did not like a literal string passed to std::bad_alloc.
        // so just throw the default-constructed exception.
        throw std::bad_alloc();
#endif
	}
    return place;
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::Deallocate
// Handles request to deallocate numBytes for 1 object.
// This will act in constant-time - except for the calls to DefaultDeallocator
// and sometimes FixedAllocator::Deallocate.  It will never throw.
////////////////////////////////////////////////////////////////////////////////

void SmallObjAllocator::Deallocate( void * p, std::size_t numBytes )
{
    if ( NULL == p ) return;
    if ( numBytes > GetMaxObjectSize() )
    {
        DefaultDeallocator( p );
        return;
    }
    assert( NULL != pool_ );
    if ( 0 == numBytes ) numBytes = 1;
    const std::size_t index = GetOffset( numBytes, GetAlignment() ) - 1;
    const std::size_t allocCount = GetOffset( GetMaxObjectSize(), GetAlignment() );
    assert( index < allocCount );
    FixedAllocator & allocator = pool_[ index ];
    assert( allocator.BlockSize() >= numBytes );
    assert( allocator.BlockSize() < numBytes + GetAlignment() );
    const bool found = allocator.Deallocate( p, true );
    assert( found );
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// March 20: fix exception safety issue in FixedAllocator::Allocate 
//     (thanks to Chris Udazvinis for pointing that out)
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Aug 02, 2002: Fix in VicinityFind sent by Pavel Vozenilek
// Nov 26, 2004: Re-implemented by Rich Sposato.
////////////////////////////////////////////////////////////////////////////////

}; // end namespace Loki

