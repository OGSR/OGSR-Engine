//-----------------------------------------------------------------------------
// boost mpl/test/next.cpp source file
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

#include "boost/mpl/next.hpp"
#include "boost/mpl/prior.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/assert_is_same.hpp"

namespace mpl = boost::mpl;
    
int main()
{
    typedef mpl::int_<0> _0;
    typedef mpl::int_<1> _1;
    typedef mpl::int_<2> _2;

    BOOST_MPL_ASSERT_IS_SAME(mpl::next<_0>::type, _1);
    BOOST_MPL_ASSERT_IS_SAME(mpl::next<_1>::type, _2);
    BOOST_MPL_ASSERT_IS_SAME(mpl::prior<_1>::type, _0);
    BOOST_MPL_ASSERT_IS_SAME(mpl::prior<_2>::type, _1);

    return 0;
}
