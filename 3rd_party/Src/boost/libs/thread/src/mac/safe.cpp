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

#include <DriverServices.h>
#include <Events.h>
#include <Multiprocessing.h>
#include <Threads.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <boost/thread/detail/force_cast.hpp>
#include <limits>
#include "execution_context.hpp"


using boost::detail::thread::force_cast;


namespace boost {

namespace threads {

namespace mac {

namespace detail {


static OSStatus safe_wait(function<OSStatus, Duration> &rFunction, Duration lDuration);


// we call WNE to allow tasks that own the resource the blue is waiting on system
//    task time, in case they are blocked on an ST remote call (or a memory allocation
//    for that matter).
static void idle()
{
    if(at_st())
    {
        EventRecord sEvent;
        bool bEvent = WaitNextEvent(0U, &sEvent, 0UL, NULL);
    }
}


OSStatus safe_wait_on_semaphore(MPSemaphoreID pSemaphoreID, Duration lDuration)
{
    function<OSStatus, Duration> oWaitOnSemaphore;
    oWaitOnSemaphore = bind(MPWaitOnSemaphore, pSemaphoreID, _1);
    return(safe_wait(oWaitOnSemaphore, lDuration));
}


OSStatus safe_enter_critical_region(MPCriticalRegionID pCriticalRegionID, Duration lDuration, MPCriticalRegionID pCriticalRegionCriticalRegionID/* = kInvalidID*/)
{
    if(pCriticalRegionCriticalRegionID != kInvalidID)
    {
        if(at_mp())
        {
        // enter the critical region's critical region
            OSStatus lStatus = noErr;
            AbsoluteTime sExpiration;
            if(lDuration != kDurationImmediate && lDuration != kDurationForever)
            {
                sExpiration = AddDurationToAbsolute(lDuration, UpTime());
            }
            lStatus = MPEnterCriticalRegion(pCriticalRegionCriticalRegionID, lDuration);
            assert(lStatus == noErr || lStatus == kMPTimeoutErr);
            if(lStatus == noErr)
            {
            // calculate a new duration
                if(lDuration != kDurationImmediate && lDuration != kDurationForever)
                {
                // check if we have any time left
                    AbsoluteTime sUpTime(UpTime());
                    if(force_cast<uint64_t>(sExpiration) > force_cast<uint64_t>(sUpTime))
                    {
                    // reset our duration to our remaining time
                        lDuration = AbsoluteDeltaToDuration(sExpiration, sUpTime);
                    }
                    else
                    {
                    // no time left
                        lDuration = kDurationImmediate;
                    }
                }
            // if we entered the critical region, exit it again
                lStatus = MPExitCriticalRegion(pCriticalRegionCriticalRegionID);
                assert(lStatus == noErr);
            }
            else
            {
            // otherwise, give up
                return(lStatus);
            }
        }
        else
        {
        // if we're at system task time, try to enter the critical region's critical
        //    region until we succeed.  MP tasks will block on this until we let it go.
            OSStatus lStatus;
            do
            {
                lStatus = MPEnterCriticalRegion(pCriticalRegionCriticalRegionID, kDurationImmediate);
            } while(lStatus == kMPTimeoutErr);
            assert(lStatus == noErr);
        }
    }

// try to enter the critical region
    function<OSStatus, Duration> oEnterCriticalRegion;
    oEnterCriticalRegion = bind(MPEnterCriticalRegion, pCriticalRegionID, _1);
    OSStatus lStatus = safe_wait(oEnterCriticalRegion, lDuration);

// if we entered the critical region's critical region to get the critical region,
//    exit the critical region's critical region.
    if(pCriticalRegionCriticalRegionID != kInvalidID && at_mp() == false)
    {
        lStatus = MPExitCriticalRegion(pCriticalRegionCriticalRegionID);
        assert(lStatus == noErr);
    }
    return(lStatus);
}

OSStatus safe_wait_on_queue(MPQueueID pQueueID, void **pParam1, void **pParam2, void **pParam3, Duration lDuration)
{
    function<OSStatus, Duration> oWaitOnQueue;
    oWaitOnQueue = bind(MPWaitOnQueue, pQueueID, pParam1, pParam2, pParam3, _1);
    return(safe_wait(oWaitOnQueue, lDuration));
}

OSStatus safe_delay_until(AbsoluteTime *pWakeUpTime)
{
    if(execution_context() == k_eExecutionContextMPTask)
    {
        return(MPDelayUntil(pWakeUpTime));
    }
    else
    {
        uint64_t ullWakeUpTime = force_cast<uint64_t>(*pWakeUpTime);

        while(force_cast<uint64_t>(UpTime()) < ullWakeUpTime)
        {
            idle();
        }

        return(noErr);
    }
}


OSStatus safe_wait(function<OSStatus, Duration> &rFunction, Duration lDuration)
{
    if(execution_context() == k_eExecutionContextMPTask)
    {
        return(rFunction(lDuration));
    }
    else
    {
        uint64_t ullExpiration = 0ULL;

    // get the expiration time in UpTime units
        if(lDuration == kDurationForever)
        {
            ullExpiration = ::std::numeric_limits<uint64_t>::max();
        }
        else if(lDuration == kDurationImmediate)
        {
            ullExpiration = force_cast<uint64_t>(UpTime());
        }
        else
        {
            AbsoluteTime sExpiration = AddDurationToAbsolute(lDuration, UpTime());
            ullExpiration = force_cast<uint64_t>(sExpiration);
        }

        OSStatus lStatus;
        bool bExpired = false;

        do
        {
            lStatus = rFunction(kDurationImmediate);
// mm - "if" #if 0'd out to allow task time to threads blocked on I/O
#if 0
            if(lStatus == kMPTimeoutErr)
#endif
            {
                idle();
            }
            if(lDuration != kDurationForever)
            {
                bExpired = (force_cast<uint64_t>(UpTime()) < ullExpiration);
            }
        } while(lStatus == kMPTimeoutErr && bExpired == false);

        return(lStatus);
    }
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
