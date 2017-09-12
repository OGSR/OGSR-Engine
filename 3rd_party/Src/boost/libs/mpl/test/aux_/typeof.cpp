//-----------------------------------------------------------------------------
// boost mpl/test/typeof.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2002
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/aux_/typeof.hpp"
#include "boost/mpl/assert_is_same.hpp"

namespace {
template< typename T > struct identity { static T type(); };
}

#define TYPEOF_CHECK(T) \
    BOOST_MPL_ASSERT_IS_SAME( \
          BOOST_MPL_AUX_TYPEOF(identity<T>::type()) \
        , T \
        ) \
/**/

int main()
{
    TYPEOF_CHECK(bool);
    TYPEOF_CHECK(signed char);
    TYPEOF_CHECK(unsigned char);
    TYPEOF_CHECK(char);
    TYPEOF_CHECK(short);
    TYPEOF_CHECK(unsigned short);
    TYPEOF_CHECK(int);
    TYPEOF_CHECK(unsigned int);
    TYPEOF_CHECK(long);
    TYPEOF_CHECK(unsigned long);
    TYPEOF_CHECK(float);
    TYPEOF_CHECK(double);
    TYPEOF_CHECK(long double);

    return 0;
}
