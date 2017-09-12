//-----------------------------------------------------------------------------
// boost mpl/test/copy_if.cpp source file
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

#include "boost/mpl/copy_if.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/push_front.hpp"
#include "boost/mpl/less.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/mpl/size.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;
using mpl::_;

int main()
{
    typedef mpl::list_c<int,0,1,2,3,4,5,6,7,8,9>::type numbers;
    typedef mpl::list_c<int,4,3,2,1,0>::type answer;
    typedef mpl::copy_if<
          numbers
        , mpl::list0_c<int>
        , mpl::push_front<_,_>
        , mpl::less<_,mpl::int_<5> >
        >::type result;

    BOOST_STATIC_ASSERT(mpl::size<result>::value == 5);
    BOOST_STATIC_ASSERT((mpl::equal<result,answer>::type::value));

    return 0;
}
