//-----------------------------------------------------------------------------
// boost mpl/test/aux_/has_rebind.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/aux_/has_rebind.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

struct f1 {};
struct f2 { struct rebind; };

int main()
{
    BOOST_STATIC_ASSERT(!mpl::aux::has_rebind<f1>::value);    
    BOOST_STATIC_ASSERT(mpl::aux::has_rebind<f2>::value);    
    return 0;
}
