//-----------------------------------------------------------------------------
// boost mpl/test/if.cpp source file
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

#include "boost/mpl/if.hpp"
#include "boost/mpl/bool.hpp"
#include "boost/mpl/assert_is_same.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::if_<mpl::true_,char,long>::type t1;
    typedef mpl::if_c<true,char,long>::type t2;
    BOOST_MPL_ASSERT_IS_SAME(t1, char);
    BOOST_MPL_ASSERT_IS_SAME(t2, char);

    typedef mpl::if_<mpl::false_,char,long>::type t3;
    typedef mpl::if_c<false,char,long>::type t4;
    BOOST_MPL_ASSERT_IS_SAME(t3, long);
    BOOST_MPL_ASSERT_IS_SAME(t4, long);

    return 0;
}
