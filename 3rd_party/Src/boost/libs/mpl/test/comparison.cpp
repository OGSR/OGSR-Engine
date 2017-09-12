//-----------------------------------------------------------------------------
// boost mpl/test/comparison.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/comparison.hpp"
#include "boost/mpl/int.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::int_<0> _0;
    typedef mpl::int_<10> _10;

    BOOST_STATIC_ASSERT((mpl::less<_0, _10>::value));
    BOOST_STATIC_ASSERT((!mpl::less<_10, _0>::value));
    BOOST_STATIC_ASSERT((!mpl::less<_10, _10>::value));

    BOOST_STATIC_ASSERT((mpl::less_equal<_0, _10>::value));
    BOOST_STATIC_ASSERT((!mpl::less_equal<_10, _0>::value));
    BOOST_STATIC_ASSERT((mpl::less_equal<_10, _10>::value));

    BOOST_STATIC_ASSERT((mpl::greater<_10, _0>::value));
    BOOST_STATIC_ASSERT((!mpl::greater<_0, _10>::value));
    BOOST_STATIC_ASSERT((!mpl::greater<_10, _10>::value));

    BOOST_STATIC_ASSERT((!mpl::greater_equal<_0, _10>::value));
    BOOST_STATIC_ASSERT((mpl::greater_equal<_10, _0>::value));
    BOOST_STATIC_ASSERT((mpl::greater_equal<_10, _10>::value));

    BOOST_STATIC_ASSERT((!mpl::equal_to<_0, _10>::value));
    BOOST_STATIC_ASSERT((!mpl::equal_to<_10, _0>::value));
    BOOST_STATIC_ASSERT((mpl::equal_to<_10, _10>::value));

    BOOST_STATIC_ASSERT((mpl::not_equal_to<_0, _10>::value));
    BOOST_STATIC_ASSERT((mpl::not_equal_to<_10, _0>::value));
    BOOST_STATIC_ASSERT((!mpl::not_equal_to<_10, _10>::value));

    return 0;
}
