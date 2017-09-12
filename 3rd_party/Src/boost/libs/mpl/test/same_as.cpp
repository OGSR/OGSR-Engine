//-----------------------------------------------------------------------------
// boost mpl/test/same_as.cpp source file
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

#include "boost/mpl/same_as.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    BOOST_STATIC_ASSERT((mpl::apply1< mpl::same_as<int>,int >::type::value));
    BOOST_STATIC_ASSERT((!mpl::apply1< mpl::same_as<int>,long >::type::value));
    return 0;
}
