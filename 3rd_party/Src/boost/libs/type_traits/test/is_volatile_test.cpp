
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_volatile)

TT_TEST_BEGIN(is_volatile)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<volatile void>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<volatile test_abc1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<volatile int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<volatile UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<volatile const UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_volatile<foo0_t>::value, false);

TT_TEST_END







