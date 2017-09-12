//-----------------------------------------------------------------------------
// boost mpl/test/aux_/has_xxx.cpp source file
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

#include "boost/mpl/aux_/has_xxx.hpp"
#include "boost/mpl/aux_/config/workaround.hpp"
#include "boost/static_assert.hpp"

BOOST_MPL_HAS_XXX_TRAIT_DEF(xxx_type)

struct a1;
struct a2 {};
struct a3 { typedef int xxx_type; };
struct a4 { struct xxx_type; };
struct a5 { typedef int& xxx_type; };
struct a6 { typedef int* xxx_type; };
struct a7 { typedef int xxx_type[10]; };
struct a8 { typedef void (*xxx_type)(); };
struct a9 { typedef void (xxx_type)(); };

int main()
{
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
    BOOST_STATIC_ASSERT(!has_xxx_type<int&>::value);
    BOOST_STATIC_ASSERT(!has_xxx_type<int*>::value);
    BOOST_STATIC_ASSERT(!has_xxx_type<int[]>::value);
    BOOST_STATIC_ASSERT(!has_xxx_type<int (*)()>::value);
#endif

    BOOST_STATIC_ASSERT(!has_xxx_type<int>::value);
    BOOST_STATIC_ASSERT(!has_xxx_type<a1>::value);
    BOOST_STATIC_ASSERT(!has_xxx_type<a2>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a3>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a4>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a5>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a6>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a7>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a8>::value);
    BOOST_STATIC_ASSERT(has_xxx_type<a9>::value);

    return 0;
}
