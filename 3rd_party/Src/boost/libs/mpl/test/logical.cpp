//-----------------------------------------------------------------------------
// boost mpl/test/logical.cpp source file
// See http://www.boost.org for updates,documentation,and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy
//
// Permission to use,copy,modify,distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/logical.hpp"
#include "boost/mpl/bool.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

struct my;
struct true_ : mpl::true_ {};
struct false_ : mpl::false_ {};

int main()
{
    BOOST_STATIC_ASSERT((mpl::and_< true_,true_ >::value == true));
    BOOST_STATIC_ASSERT((mpl::and_< false_,true_ >::value == false));
    BOOST_STATIC_ASSERT((mpl::and_< true_,false_ >::value == false));
    BOOST_STATIC_ASSERT((mpl::and_< false_,false_ >::value == false));
    BOOST_STATIC_ASSERT((mpl::and_< false_,my >::value == false));
    BOOST_STATIC_ASSERT((mpl::and_< false_,my,my >::value == false));

    BOOST_STATIC_ASSERT((mpl::or_< true_,true_ >::value == true));
    BOOST_STATIC_ASSERT((mpl::or_< false_,true_ >::value == true));
    BOOST_STATIC_ASSERT((mpl::or_< true_,false_ >::value == true));
    BOOST_STATIC_ASSERT((mpl::or_< false_,false_ >::value == false));
    BOOST_STATIC_ASSERT((mpl::or_< true_,my >::value == true));
    BOOST_STATIC_ASSERT((mpl::or_< true_,my,my >::value == true));

    BOOST_STATIC_ASSERT((mpl::not_< true_ >::value == false));
    BOOST_STATIC_ASSERT((mpl::not_< false_ >::value == true));
  
    return 0;
}
