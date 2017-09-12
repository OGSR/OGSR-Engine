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
#include <cstdio>

#include <MacTypes.h>

#include "remote_calls.hpp"

// this function will be called when an assertion fails.  We redirect the assertion
//    to DebugStr (MacsBug under Mac OS 1.x-9.x, Console under Mac OS X).
void __assertion_failed(char const *pszAssertion, char const *pszFile, int nLine)
{
    using std::snprintf;
    unsigned char strlDebug[sizeof(Str255) + 1];
    char *pszDebug = reinterpret_cast<char *>(&strlDebug[1]);
    strlDebug[0] = snprintf(pszDebug, sizeof(Str255), "assertion failed: \"%s\", %s, line %d", pszAssertion, pszFile, nLine);
    boost::threads::mac::dt_remote_call(DebugStr, static_cast<ConstStringPtr>(strlDebug));
}
