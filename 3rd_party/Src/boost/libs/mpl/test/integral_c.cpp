//-----------------------------------------------------------------------------
// boost mpl/test/integral_c.cpp source file
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

#include "boost/mpl/integral_c.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/static_assert.hpp"
#include "boost/preprocessor/repeat.hpp"

#include <cassert>

namespace mpl = boost::mpl;

#define INTEGRAL_C_TEST(z, i, T)                                \
    {                                                           \
        typedef mpl::integral_c<T,i> c##i;                      \
        typedef mpl::integral_c<T,i+1> next_c##i;               \
        typedef mpl::integral_c<T,i-1> prior_c##i;              \
        { BOOST_MPL_ASSERT_IS_SAME(c##i::value_type, T); }      \
        { BOOST_MPL_ASSERT_IS_SAME(c##i::type, c##i); }         \
        { BOOST_MPL_ASSERT_IS_SAME(c##i::next, next_c##i); }    \
        { BOOST_MPL_ASSERT_IS_SAME(c##i::prior, prior_c##i); }  \
        { BOOST_STATIC_ASSERT(c##i::value == i); }              \
        assert(c##i() == i);                                    \
    }
/**/

int main()
{
    BOOST_PP_REPEAT(10, INTEGRAL_C_TEST, char)
    BOOST_PP_REPEAT(10, INTEGRAL_C_TEST, short)
    BOOST_PP_REPEAT(10, INTEGRAL_C_TEST, int)
    return 0;
}
