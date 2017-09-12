//-----------------------------------------------------------------------------
// boost mpl/test/find_if.cpp source file
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

#include "boost/mpl/find_if.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/distance.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/type_traits/is_float.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    using namespace mpl::placeholders;
    typedef mpl::list9<int,char,long,short,char,long,double,float,char>::type types;

    typedef mpl::find_if< types, boost::is_float<_> >::type iter1;
    typedef mpl::find_if< types, boost::is_same<_,long> >::type iter2;
    typedef mpl::find_if< types, boost::is_same<_,void> >::type iter3;

    BOOST_MPL_ASSERT_IS_SAME(iter1::type, double);
    BOOST_MPL_ASSERT_IS_SAME(iter2::type, long);
    BOOST_MPL_ASSERT_IS_SAME(iter3, mpl::end<types>::type);

    typedef mpl::begin<types>::type first;
    BOOST_STATIC_ASSERT((mpl::distance<first,iter1>::type::value == 6));
    BOOST_STATIC_ASSERT((mpl::distance<first,iter2>::type::value == 2));

    return 0;
}
