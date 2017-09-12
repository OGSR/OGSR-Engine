//-----------------------------------------------------------------------------
// boost mpl/test/quote.cpp source file
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

#include "boost/mpl/quote.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"

using namespace boost::mpl;

template< typename T > struct f1
{
    typedef T type;
};

template<
      typename T1, typename T2, typename T3, typename T4, typename T5
    >
struct f5
{
    // no 'type' member!
};

int main()
{
    typedef apply1< quote1<f1>,int >::type t1;
    typedef apply5< quote5<f5>,char,short,int,long,float >::type t5;
    
    BOOST_STATIC_ASSERT((boost::is_same< t1, int >::value));
    BOOST_STATIC_ASSERT((boost::is_same< t5, f5<char,short,int,long,float> >::value));

    return 0;
}
