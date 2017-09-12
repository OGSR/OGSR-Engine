//-----------------------------------------------------------------------------
// boost mpl/test/empty.cpp source file
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

#include "boost/mpl/empty.hpp"
#include "boost/mpl/list/list10.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list0<> list0;
    typedef mpl::list1<char> list1;
    
    BOOST_STATIC_ASSERT((mpl::empty<list0>::type::value == true));
    BOOST_STATIC_ASSERT((mpl::empty<list1>::type::value == false));

    return 0;
}
