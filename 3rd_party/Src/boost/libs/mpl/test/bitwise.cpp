
// + file: libs/mpl/test/bitwise.cpp
// + last modified: 09/mar/03

// Copyright (c) 2001-03
// Aleksey Gurtovoy, Jaap Suter
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.
//
// See http://www.boost.org/libs/mpl for documentation.

#include "boost/mpl/bitwise.hpp"
#include "boost/mpl/integral_c.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::integral_c<unsigned int, 0> _0;
    typedef mpl::integral_c<unsigned int, 1> _1;
    typedef mpl::integral_c<unsigned int, 2> _2;
    typedef mpl::integral_c<unsigned int, 8> _8;
    typedef mpl::integral_c<unsigned int, 0xFFFFFFFF> _FFFFFFFF;

    BOOST_STATIC_ASSERT((mpl::bitand_<_0,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitand_<_1,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitand_<_0,_1>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitand_<_0,_FFFFFFFF>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitand_<_1,_FFFFFFFF>::value == 1));
    BOOST_STATIC_ASSERT((mpl::bitand_<_8,_FFFFFFFF>::value == 8));

    BOOST_STATIC_ASSERT((mpl::bitor_<_0,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitor_<_1,_0>::value == 1));
    BOOST_STATIC_ASSERT((mpl::bitor_<_0,_1>::value == 1));
    BOOST_STATIC_ASSERT((mpl::bitor_<_0,_FFFFFFFF>::value == 0xFFFFFFFF));
    BOOST_STATIC_ASSERT((mpl::bitor_<_1,_FFFFFFFF>::value == 0xFFFFFFFF));
    BOOST_STATIC_ASSERT((mpl::bitor_<_8,_FFFFFFFF>::value == 0xFFFFFFFF));

    BOOST_STATIC_ASSERT((mpl::bitxor_<_0,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::bitxor_<_1,_0>::value == 1));
    BOOST_STATIC_ASSERT((mpl::bitxor_<_0,_1>::value == 1));
    BOOST_STATIC_ASSERT((mpl::bitxor_<_0,_FFFFFFFF>::value == (0xFFFFFFFF ^ 0)));
    BOOST_STATIC_ASSERT((mpl::bitxor_<_1,_FFFFFFFF>::value == (0xFFFFFFFF ^ 1)));
    BOOST_STATIC_ASSERT((mpl::bitxor_<_8,_FFFFFFFF>::value == (0xFFFFFFFF ^ 8)));

    BOOST_STATIC_ASSERT((mpl::shift_right<_0,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::shift_right<_1,_0>::value == 1));
    BOOST_STATIC_ASSERT((mpl::shift_right<_1,_1>::value == 0));
    BOOST_STATIC_ASSERT((mpl::shift_right<_2,_1>::value == 1));
    BOOST_STATIC_ASSERT((mpl::shift_right<_8,_1>::value == 4));

    BOOST_STATIC_ASSERT((mpl::shift_left<_0,_0>::value == 0));
    BOOST_STATIC_ASSERT((mpl::shift_left<_1,_0>::value == 1));
    BOOST_STATIC_ASSERT((mpl::shift_left<_1,_1>::value == 2));
    BOOST_STATIC_ASSERT((mpl::shift_left<_2,_1>::value == 4));
    BOOST_STATIC_ASSERT((mpl::shift_left<_8,_1>::value == 16));

    return 0;
}
