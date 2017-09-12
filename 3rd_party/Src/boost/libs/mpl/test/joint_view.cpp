//-----------------------------------------------------------------------------
// boost mpl/test/joint_view.cpp source file
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

#include "boost/mpl/joint_view.hpp"
#include "boost/mpl/range_c.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::joint_view<
          mpl::range_c<int,0,10>
        , mpl::range_c<int,10,15>
        > numbers;

    typedef mpl::range_c<int,0,15> answer;
    BOOST_STATIC_ASSERT((mpl::equal<numbers,answer>::type::value));

    return 0;
}
