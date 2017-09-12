//-----------------------------------------------------------------------------
// boost mpl/test/erase.cpp source file
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

#include "boost/mpl/erase.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/list_c.hpp"
#include "boost/mpl/find.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/integral_c.hpp"
#include "boost/mpl/begin_end.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::list<int,char,long,short,char,long,double,long> types;
    typedef mpl::list_c<int,1,0,5,1,7,5,0,5> values;

    typedef mpl::find<types,short>::type types_iter;
    typedef mpl::find< values, mpl::integral_c<int,7> >::type values_iter;

    typedef mpl::erase<types, types_iter>::type result_types;
    typedef mpl::erase<values, values_iter>::type result_values;

    BOOST_STATIC_ASSERT(mpl::size<result_types>::type::value == 7);
    BOOST_STATIC_ASSERT(mpl::size<result_values>::type::value == 7);

    typedef mpl::find<result_types,short>::type result_types_iter;
    typedef mpl::find<result_values, mpl::integral_c<int,7> >::type result_values_iter;

    BOOST_MPL_ASSERT_IS_SAME(result_types_iter, mpl::end<result_types>::type);
    BOOST_MPL_ASSERT_IS_SAME(result_values_iter, mpl::end<result_values>::type);

    return 0;
}
