//-----------------------------------------------------------------------------
// boost mpl/test/always.cpp source file
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

#include "boost/mpl/always.hpp"
#include "boost/mpl/bool.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::always<mpl::true_> always_true;
    typedef mpl::apply1< always_true,mpl::false_ >::type res1;
    typedef mpl::apply2< always_true,mpl::false_,mpl::false_ >::type res2;
    typedef mpl::apply3< always_true,mpl::false_,mpl::false_,mpl::false_ >::type res3;

    BOOST_STATIC_ASSERT(res1::value == true);
    BOOST_STATIC_ASSERT(res2::value == true);
    BOOST_STATIC_ASSERT(res3::value == true);

    typedef mpl::always< mpl::int_<10> > always_10;
    typedef mpl::apply1< always_10,mpl::int_<0> >::type res4;
    typedef mpl::apply2< always_10,mpl::int_<0>,mpl::int_<0> >::type res5;
    typedef mpl::apply3< always_10,mpl::int_<0>,mpl::int_<0>,mpl::int_<0> >::type res6;

    BOOST_STATIC_ASSERT(res4::value == 10);
    BOOST_STATIC_ASSERT(res5::value == 10);
    BOOST_STATIC_ASSERT(res6::value == 10);
    
    return 0;
}
