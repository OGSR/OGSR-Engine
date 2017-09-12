//-----------------------------------------------------------------------------
// boost mpl/test/equal.cpp source file
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

#include "boost/mpl/equal.hpp"
#include "boost/mpl/list.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list<int,float,long,double,char,long,double,float> list1;
    typedef mpl::list<int,float,long,double,char,long,double,float> list2;
    typedef mpl::list<int,float,long,double,char,long,double,short> list3;
    typedef mpl::list<int,float,long,double,char,long,double> list4;

    BOOST_STATIC_ASSERT((mpl::equal<list1,list2>::type::value == true));
    BOOST_STATIC_ASSERT((mpl::equal<list2,list1>::type::value == true));
    BOOST_STATIC_ASSERT((mpl::equal<list2,list3>::type::value == false));
    BOOST_STATIC_ASSERT((mpl::equal<list3,list4>::type::value == false));
    BOOST_STATIC_ASSERT((mpl::equal<list4,list3>::type::value == false));

    return 0;
}
