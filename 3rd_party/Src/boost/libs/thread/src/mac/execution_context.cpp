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

#include <Debugging.h>
#include <Multiprocessing.h>

#include "execution_context.hpp"
#include "init.hpp"


namespace boost {

namespace threads {

namespace mac {


execution_context_t execution_context()
{
// make sure that MP services are available the first time through
    static bool bIgnored = detail::thread_init();

// first check if we're an MP task
    if(MPTaskIsPreemptive(kInvalidID))
    {
        return(k_eExecutionContextMPTask);
    }

#if TARGET_CARBON
// Carbon has TaskLevel
    UInt32 ulLevel = TaskLevel();

    if(ulLevel == 0UL)
    {
        return(k_eExecutionContextSystemTask);
    }

    if(ulLevel & kInDeferredTaskMask)
    {
        return(k_eExecutionContextDeferredTask);
    }

    return(k_eExecutionContextOther);
#else
// this can be implemented using TaskLevel if you don't mind linking against
//    DebugLib (and therefore breaking Mac OS 8.6 support), or CurrentExecutionLevel.
#    error execution_context unimplimented
#endif
}


} // namespace mac

} // namespace threads

} // namespace boost
