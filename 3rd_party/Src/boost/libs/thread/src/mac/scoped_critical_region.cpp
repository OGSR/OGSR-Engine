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

#include "scoped_critical_region.hpp"

#include "init.hpp"


#include <cassert>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


scoped_critical_region::scoped_critical_region():
    m_pCriticalRegionID(kInvalidID)
{
    static bool bIgnored = thread_init();
    OSStatus lStatus = MPCreateCriticalRegion(&m_pCriticalRegionID);
    if(lStatus != noErr || m_pCriticalRegionID == kInvalidID)
        throw(thread_resource_error());
}

scoped_critical_region::~scoped_critical_region()
{
    OSStatus lStatus = MPDeleteCriticalRegion(m_pCriticalRegionID);
    assert(lStatus == noErr);
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
