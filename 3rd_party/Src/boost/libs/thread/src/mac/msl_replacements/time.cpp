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

#include <cassert>
// we include timesize.mac.h to get whether or not __TIMESIZE_DOUBLE__ is
//  defined.  This is not safe, given that __TIMESIZE_DOUBLE__ affects MSL
//  at MSL's compile time, not ours, so be forgiving if you have changed it
//  since you have built MSL.
#include <timesize.mac.h>
#include <time.h>

#include <boost/thread/detail/force_cast.hpp>
#include <boost/thread/xtime.hpp>

#include "execution_context.hpp"

#include <DriverServices.h>


extern "C"
{
    clock_t __get_clock();
    time_t __get_time();
    int __to_gm_time(time_t *pTime);
    int __is_dst();
}


static inline uint64_t get_nanoseconds()
{
    using boost::detail::thread::force_cast;
    return(force_cast<uint64_t>(AbsoluteToNanoseconds(UpTime())));
}


#ifdef __TIMESIZE_DOUBLE__

// return number of microseconds since startup as a double
clock_t __get_clock()
{
    static const double k_dNanosecondsPerMicrosecond(1000.0);

    return(get_nanoseconds() / k_dNanosecondsPerMicrosecond);
}

#else

// return number of ticks (60th of a second) since startup as a long
clock_t __get_clock()
{
    static const uint64_t k_ullTicksPerSecond(60ULL);
    static const uint64_t k_ullNanosecondsPerSecond(1000ULL * 1000ULL * 1000ULL);
    static const uint64_t k_ullNanosecondsPerTick(k_ullNanosecondsPerSecond / k_ullTicksPerSecond);

    return(get_nanoseconds() / k_ullNanosecondsPerTick);
}

#endif

// return number of seconds elapsed since Jan 1, 1970
time_t __get_time()
{
    boost::xtime sTime;
    int nType = boost::xtime_get(&sTime, boost::TIME_UTC);
    assert(nType == boost::TIME_UTC);
    return(static_cast<time_t>(sTime.sec));
}


static inline MachineLocation &read_location()
{
    static MachineLocation s_sLocation;
    assert(boost::threads::mac::at_st());
    ReadLocation(&s_sLocation);
    return(s_sLocation);
}

static inline MachineLocation &get_location()
{
    static MachineLocation &s_rLocation(read_location());
    return(s_rLocation);
}


// force the machine location to be cached at static initlialization
static MachineLocation &g_rIgnored(get_location());

static inline long calculate_delta()
{
    MachineLocation &rLocation(get_location());

// gmtDelta is a 24-bit, signed integer.  We need to strip out the lower 24 bits,
//  then sign-extend what we have.
    long lDelta = rLocation.u.gmtDelta & 0x00ffffffL;
    if((lDelta & 0x00800000L) != 0L)
    {
        lDelta |= 0xFF000000;
    }
    return(lDelta);
}

static inline bool check_if_location_is_broken()
{
    MachineLocation &rLocation(get_location());
    if(rLocation.latitude == 0 && rLocation.longitude == 0 && rLocation.u.gmtDelta == 0)
        return(true);
    return(false);
}

static inline bool location_is_broken()
{
    static bool s_bLocationIsBroken(check_if_location_is_broken());
    return(s_bLocationIsBroken);
}


// translate time to GMT
int __to_gm_time(time_t *pTime)
{
    if(location_is_broken())
    {
        return(0);
    }

    static long s_lDelta(calculate_delta());
    *pTime -= s_lDelta;
    return(1);
}


static inline bool is_daylight_savings_time()
{
    MachineLocation &rLocation(get_location());
    return(rLocation.u.dlsDelta != 0);
}

// check if we're in daylight savings time
int __is_dst()
{
    if(location_is_broken())
    {
        return(-1);
    }
    static bool bIsDaylightSavingsTime(is_daylight_savings_time());
    return(static_cast<int>(bIsDaylightSavingsTime));
}
