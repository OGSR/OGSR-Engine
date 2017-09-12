//-----------------------------------------------------------------------------
// boost mpl/test/range_c.cpp source file
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

#include "boost/mpl/range_c.hpp"
#include "boost/mpl/empty.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/back.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::range_c<int,0,0> range0;
    typedef mpl::range_c<int,0,1> range1;
    typedef mpl::range_c<int,0,10> range10;

    BOOST_STATIC_ASSERT(mpl::size<range0>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::size<range1>::type::value == 1);
    BOOST_STATIC_ASSERT(mpl::size<range10>::type::value == 10);

    BOOST_STATIC_ASSERT(mpl::empty<range0>::type::value);
    BOOST_STATIC_ASSERT(!mpl::empty<range1>::type::value);
    BOOST_STATIC_ASSERT(!mpl::empty<range10>::type::value);

    BOOST_MPL_ASSERT_IS_SAME(mpl::begin<range0>::type, mpl::end<range0>::type);
    BOOST_MPL_ASSERT_NOT_SAME(mpl::begin<range1>::type, mpl::end<range1>::type);
    BOOST_MPL_ASSERT_NOT_SAME(mpl::begin<range10>::type, mpl::end<range10>::type);

    BOOST_STATIC_ASSERT(mpl::front<range1>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::back<range1>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::front<range10>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::back<range10>::type::value == 9);

    return 0;
}
