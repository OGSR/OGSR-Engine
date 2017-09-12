//-----------------------------------------------------------------------------
// boost mpl/test/lower_bound.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/lower_bound.hpp"
#include "boost/mpl/distance.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/less.hpp"
#include "boost/mpl/int.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list_c<int,1,2,3,3,3,5,8> numbers;
    typedef mpl::lower_bound< numbers, mpl::int_<3> >::type iter;
    BOOST_STATIC_ASSERT((mpl::distance< mpl::begin<numbers>::type,iter >::type::value == 2));
    BOOST_STATIC_ASSERT(iter::type::value == 3);
    return 0;
}
