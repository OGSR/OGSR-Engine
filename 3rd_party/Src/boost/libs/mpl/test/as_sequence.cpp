//-----------------------------------------------------------------------------
// boost mpl/test/as_sequence.cpp source file
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

#include "boost/mpl/vector.hpp"
#include "boost/mpl/as_sequence.hpp"
#include "boost/static_assert.hpp"

using namespace boost::mpl;

struct UDT {};

int main()
{
    BOOST_STATIC_ASSERT(is_sequence< as_sequence<int>::type >::value);
    BOOST_STATIC_ASSERT(is_sequence< as_sequence<UDT>::type >::value);
    BOOST_STATIC_ASSERT(is_sequence< as_sequence< vector<> >::type >::value);

    return 0;
}
