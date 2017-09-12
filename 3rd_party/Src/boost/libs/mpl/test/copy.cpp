//-----------------------------------------------------------------------------
// boost mpl/test/copy.cpp source file
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

#include "boost/mpl/copy.hpp"
#include "boost/mpl/vector/vector10_c.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/range_c.hpp"
#include "boost/mpl/push_front.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;
using mpl::_;

int main()
{
    typedef mpl::vector10_c<int,9,8,7,6,5,4,3,2,1,0> answer;
    typedef mpl::copy<
          mpl::range_c<int,0,10>
        , mpl::vector<>
        , mpl::push_front<_,_>
        >::type result;

    BOOST_STATIC_ASSERT(mpl::size<result>::value == 10);
    BOOST_STATIC_ASSERT((mpl::equal< result,answer >::type::value));

    return 0;
}
