//-----------------------------------------------------------------------------
// boost mpl/test/aux_/has_distance.cpp source file
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

#include "boost/mpl/aux_/has_distance.hpp"
#include "boost/mpl/aux_/has_distance_spec.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

struct iter0 {};
template< typename T > struct iter1
{
    template< typename > struct BOOST_MPL_AUX_ITERATOR_DISTANCE;
};

namespace boost { namespace mpl {
BOOST_MPL_AUX_HAS_DISTANCE_SPEC(1,iter1)
}}

int main()
{
    BOOST_STATIC_ASSERT(!mpl::aux::has_distance<iter0>::value);    
    BOOST_STATIC_ASSERT(mpl::aux::has_distance< iter1<int> >::value);    
    return 0;
}
