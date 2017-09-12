//-----------------------------------------------------------------------------
// boost mpl/test/sizeof.cpp source file
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

#include "boost/mpl/sizeof.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

struct my
{
    char a[100];
};

int main()
{
    BOOST_STATIC_ASSERT(mpl::sizeof_<char>::type::value == sizeof(char));
    BOOST_STATIC_ASSERT(mpl::sizeof_<int>::type::value == sizeof(int));
    BOOST_STATIC_ASSERT(mpl::sizeof_<double>::type::value == sizeof(double));
    BOOST_STATIC_ASSERT(mpl::sizeof_<my>::type::value == sizeof(my));
    return 0;
}
