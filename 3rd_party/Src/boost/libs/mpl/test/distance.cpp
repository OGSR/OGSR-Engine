//-----------------------------------------------------------------------------
// boost mpl/test/distance.cpp source file
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

#include "boost/mpl/distance.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/range_c.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

void list_distance_test()
{
    typedef mpl::list<char,short,int,long>::type list;
    typedef mpl::begin<list>::type first;
    typedef mpl::end<list>::type last;
    
    BOOST_STATIC_ASSERT((mpl::distance<first,last>::type::value == 4));
}

void range_distance_test()
{
    typedef mpl::range_c<int,0,10>::type range;
    typedef mpl::begin<range>::type first;
    typedef mpl::end<range>::type last;
    
    BOOST_STATIC_ASSERT((mpl::distance<first,last>::type::value == 10));
}

int main()
{
    return 0;
}
