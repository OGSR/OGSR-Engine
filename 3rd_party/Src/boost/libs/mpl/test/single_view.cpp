//-----------------------------------------------------------------------------
// boost mpl/test/single_view.cpp source file
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

#include "boost/mpl/single_view.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/begin_end.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/static_assert.hpp"

using namespace boost::mpl;

int main()
{
    typedef single_view<int> view;
    typedef begin<view>::type first;
    typedef end<view>::type last;

    BOOST_MPL_ASSERT_IS_SAME(first::type,int);
    BOOST_MPL_ASSERT_IS_SAME(first::next,last);
    BOOST_MPL_ASSERT_IS_SAME(last::prior,first);

    BOOST_STATIC_ASSERT(size<view>::type::value == 1);

    return 0;
}
