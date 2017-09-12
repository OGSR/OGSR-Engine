//-----------------------------------------------------------------------------
// boost mpl/test/inherit.cpp source file
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

#include "boost/mpl/inherit.hpp"
#include "boost/mpl/assert_is_same.hpp"

struct her { typedef her herself; };
struct my { typedef my myself; };

int main()
{
    using namespace boost::mpl;

    BOOST_MPL_ASSERT_IS_SAME(inherit<her>::type, her);

    typedef inherit<her,my>::type her_my1;
    BOOST_MPL_ASSERT_IS_SAME(her_my1::herself, her);
    BOOST_MPL_ASSERT_IS_SAME(her_my1::myself, my);
    
    typedef inherit<empty_base,her>::type her1;
    BOOST_MPL_ASSERT_IS_SAME(her1, her);
    
    typedef inherit<empty_base,her,empty_base,empty_base>::type her2;
    BOOST_MPL_ASSERT_IS_SAME(her2, her);

    typedef inherit<her,empty_base,my>::type her_my2;
    BOOST_MPL_ASSERT_IS_SAME(her_my2::herself, her);
    BOOST_MPL_ASSERT_IS_SAME(her_my2::myself, my);

    typedef inherit<empty_base,empty_base>::type empty;
    BOOST_MPL_ASSERT_IS_SAME(empty, empty_base);

    return 0;
}
