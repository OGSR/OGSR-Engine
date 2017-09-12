//-----------------------------------------------------------------------------
// boost mpl/test/fold.cpp source file
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

#include "boost/mpl/fold.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/if.hpp"
#include "boost/mpl/integral_c.hpp"
#include "boost/type_traits/is_float.hpp"
#include "boost/static_assert.hpp"

using namespace boost::mpl;

int main()
{
    typedef vector<long,float,short,double,float,long,long double> types;
    typedef fold<
          types
        , integral_c<long, 0>
        , if_< boost::is_float<_2>,next<_1>,_1 >
        >::type number_of_floats;

    BOOST_STATIC_ASSERT(number_of_floats::value == 4);

    return 0;
}
