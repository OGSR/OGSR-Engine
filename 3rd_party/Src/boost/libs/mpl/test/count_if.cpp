//-----------------------------------------------------------------------------
// boost mpl/test/count_if.cpp source file
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

#include "boost/mpl/count_if.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/less.hpp"
#include "boost/mpl/equal_to.hpp"
#include "boost/type_traits/is_float.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    using namespace mpl::placeholders;
    typedef mpl::list<int,char&,long,short,char&,long,double,long> types;
    typedef mpl::list_c<int,1,0,5,1,7,5,0,5> values;
    
    BOOST_STATIC_ASSERT((mpl::count_if< types, boost::is_float<_> >::type::value == 1));
    BOOST_STATIC_ASSERT((mpl::count_if< types, boost::is_same<_,char&> >::type::value == 2));
    BOOST_STATIC_ASSERT((mpl::count_if< types, boost::is_same<_,void*> >::type::value == 0));

    BOOST_STATIC_ASSERT((mpl::count_if< values, mpl::lt<5> >::type::value == 4));
    BOOST_STATIC_ASSERT((mpl::count_if< values, mpl::eq<0>  >::type::value == 2));
    BOOST_STATIC_ASSERT((mpl::count_if< values, mpl::eq<-1> >::type::value == 0));
    
    return 0;
}
