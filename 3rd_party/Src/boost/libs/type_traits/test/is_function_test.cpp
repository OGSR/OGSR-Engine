
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_function)

TT_TEST_BEGIN(is_function)

typedef void foo0_t();
typedef void foo1_t(int);
typedef void foo2_t(int&, double);
typedef void foo3_t(int&, bool, int, int);
typedef void foo4_t(int, bool, int*, int[], int, int, int, int, int);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<foo0_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<foo1_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<foo2_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<foo3_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<foo4_t>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<int*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<int[]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<int (*)(int)>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<incomplete_type>::value, false);

TT_TEST_END







