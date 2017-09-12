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

#include "os.hpp"


#include <cassert>

#include <Gestalt.h>


namespace boost {

namespace threads {

namespace mac {

namespace os {


// read the OS version from Gestalt
static inline long get_version()
{
    long lVersion;
    OSErr nErr = Gestalt(gestaltSystemVersion, &lVersion);
    assert(nErr == noErr);
    return(lVersion);
}


// check if we're running under Mac OS X and cache that information
bool x()
{
    static bool bX = (version() >= 0x1000);
    return(bX);
}


// read the OS version and cache it
long version()
{
    static long lVersion = get_version();
    return(lVersion);
}


} // namespace os

} // namespace mac

} // namespace threads

} // namespace boost
