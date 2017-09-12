
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_const)

TT_TEST_BEGIN(is_const)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const void>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const test_abc1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const volatile UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<const int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<cr_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_const<foo0_t>::value, false);

TT_TEST_END







