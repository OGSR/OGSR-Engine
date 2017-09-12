//-----------------------------------------------------------------------------
// boost mpl/test/copy_backward_if.cpp source file
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

#include "boost/mpl/copy_backward_if.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/push_front.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/type_traits/is_float.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;
using mpl::_;

int main()
{
    typedef mpl::list<int,float,long,float,char,long,double,double>::type types;
    typedef mpl::list<float,float,double,double>::type float_types;
    typedef mpl::copy_backward_if<
          types
        , mpl::list0<>
        , mpl::push_front<_,_>
        , boost::is_float<_>
        >::type result;

    BOOST_STATIC_ASSERT(mpl::size<result>::value == 4);
    BOOST_STATIC_ASSERT((mpl::equal<result,result>::type::value));

    return 0;
}
