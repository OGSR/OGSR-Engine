//-----------------------------------------------------------------------------
// boost mpl/test/advance.cpp source file
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

#include "boost/mpl/vector/vector10_c.hpp"
#include "boost/mpl/advance.hpp"
#include "boost/mpl/begin_end.hpp"
#include "boost/mpl/assert_is_same.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::vector10_c<int,0,1,2,3,4,5,6,7,8,9> numbers;
    typedef mpl::begin<numbers>::type first;
    typedef mpl::end<numbers>::type last;

    typedef mpl::advance_c<first,10>::type iter1;
    typedef mpl::advance_c<last,-10>::type iter2;

    BOOST_MPL_ASSERT_IS_SAME(iter1, last);
    BOOST_MPL_ASSERT_IS_SAME(iter2, first);
        
    return 0;
}
