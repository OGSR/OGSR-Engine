//-----------------------------------------------------------------------------
// boost mpl/test/erase_range.cpp source file
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

#include "boost/mpl/erase.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/find.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/begin_end.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/static_assert.hpp"
#include "boost/config.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list<int,char,long,short,unsigned,long,double,long> types;

    typedef mpl::find<types,short>::type iter1;
    typedef mpl::find<types,double>::type iter2;

    typedef mpl::erase<types,iter1,iter2>::type result;
    
    BOOST_STATIC_ASSERT(mpl::size<result>::type::value == 5);

    typedef mpl::find<result,unsigned>::type iter;
    BOOST_MPL_ASSERT_IS_SAME(iter, mpl::end<result>::type);

    return 0;
}
