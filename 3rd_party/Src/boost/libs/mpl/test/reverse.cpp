//-----------------------------------------------------------------------------
// boost mpl/test/reverse.cpp source file
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

#include "boost/mpl/reverse.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/range_c.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/mpl/equal_to.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    using namespace mpl::placeholders;
    typedef mpl::list_c<int,9,8,7,6,5,4,3,2,1,0>::type numbers;
    typedef mpl::reverse< numbers >::type result;

    typedef mpl::range_c<int,0,10> answer;
    BOOST_STATIC_ASSERT((mpl::equal< result,answer,mpl::equal_to<_,_> >::type::value));

    return 0;
}
