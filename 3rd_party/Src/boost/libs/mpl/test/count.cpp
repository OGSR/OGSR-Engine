//-----------------------------------------------------------------------------
// boost mpl/test/count.cpp source file
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

#include "boost/mpl/count.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/int.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list<int,char,long,short,char,long,double,long> types;
    typedef mpl::list_c<int,1,0,5,1,7,5,0,5> values;

    BOOST_STATIC_ASSERT((mpl::count<types,int>::type::value == 1));
    BOOST_STATIC_ASSERT((mpl::count<types,double>::type::value == 1));
    BOOST_STATIC_ASSERT((mpl::count<types,char>::type::value == 2));
    BOOST_STATIC_ASSERT((mpl::count<types,long>::type::value == 3));
    BOOST_STATIC_ASSERT((mpl::count<types,unsigned>::type::value == 0));
    
    BOOST_STATIC_ASSERT((mpl::count< values, mpl::integral_c<int,1> >::type::value == 2));
    BOOST_STATIC_ASSERT((mpl::count< values, mpl::integral_c<int,0> >::type::value == 2));
    BOOST_STATIC_ASSERT((mpl::count< values, mpl::integral_c<int,5> >::type::value == 3));
    BOOST_STATIC_ASSERT((mpl::count< values, mpl::integral_c<int,7> >::type::value == 1));
    BOOST_STATIC_ASSERT((mpl::count< values, mpl::integral_c<int,8> >::type::value == 0));
    
    return 0;
}
