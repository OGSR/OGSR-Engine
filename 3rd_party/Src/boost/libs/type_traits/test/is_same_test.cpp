
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_COMPARE(is_same)

TT_TEST_BEGIN(is_same)

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int, int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int, const int>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int, int&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<const int, int&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int, const int&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int*, const int*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int*, int*const>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int, int[2]>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int*, int[2]>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_same<int[4], int[2]>::value), false);

TT_TEST_END







