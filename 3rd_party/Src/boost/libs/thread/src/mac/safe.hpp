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

#ifndef BOOST_SAFE_MJM012402_HPP
#define BOOST_SAFE_MJM012402_HPP


#include <Multiprocessing.h>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// these functions are used to wain in an execution context-independent manor.  All of these
//  functions are both MP- and ST-safe.

OSStatus safe_wait_on_semaphore(MPSemaphoreID pSemaphoreID, Duration lDuration);
OSStatus safe_enter_critical_region(MPCriticalRegionID pCriticalRegionID, Duration lDuration, MPCriticalRegionID pCriticalRegionCriticalRegionID = kInvalidID);
OSStatus safe_wait_on_queue(MPQueueID pQueueID, void **pParam1, void **pParam2, void **pParam3, Duration lDuration);
OSStatus safe_delay_until(AbsoluteTime *pWakeUpTime);


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost

#endif // BOOST_SAFE_MJM012402_HPP
