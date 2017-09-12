//-----------------------------------------------------------------------------
// boost mpl/test/replace.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy and John R. Bandela
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/replace.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list<int,float,char,float,float,double>::type types;
    typedef mpl::replace< types,float,double >::type result;

    typedef mpl::list<int,double,char,double,double,double>::type answer;
    BOOST_STATIC_ASSERT((mpl::equal< result,answer >::type::value));

    return 0;
}
