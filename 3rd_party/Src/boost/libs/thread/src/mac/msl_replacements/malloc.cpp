// Copyright (C) 2001
// Mac Murrett
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Mac Murrett makes no representations
// about the suitability of this software for any purpose.  It is
// provided "as is" without express or implied warranty.
//
// See http://www.boost.org for most recent version including documentation.

//
// includes
//

#include <cstdlib>

#include <Multiprocessing.h>


//
// using declarations
//

using std::size_t;


extern "C" {

//
// prototypes
//

void *malloc(size_t ulSize);
void free(void *pBlock);

}


//
// MSL function replacements
//

// all allocation/deallocation currently goes through MPAllocateAligned/MPFree.  This
//    solution is sub-optimal at best, but will have to do for now.
void *malloc(size_t ulSize)
{
    static bool bIgnored = MPLibraryIsLoaded();
    return(MPAllocateAligned(ulSize, kMPAllocateDefaultAligned, 0UL));
}

void free(void *pBlock)
{
    if(pBlock == NULL) return;
    MPFree(pBlock);
}
