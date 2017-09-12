//-----------------------------------------------------------------------------
// boost mpl/test/assert_is_same.cpp source file
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

#include "boost/mpl/assert_is_same.hpp"

namespace {
struct my;
}

int main()
{
    BOOST_MPL_ASSERT_IS_SAME(long, long);
    BOOST_MPL_ASSERT_IS_SAME(my, my);

    BOOST_MPL_ASSERT_NOT_SAME(my, long);
    BOOST_MPL_ASSERT_NOT_SAME(long, my);

    return 0;
}
