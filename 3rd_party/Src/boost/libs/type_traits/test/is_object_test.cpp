
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_object)

TT_TEST_BEGIN(is_object)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<test_abc1>::value, true);
// this one is only partly correct:
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_object<incomplete_type>::value, true);


TT_TEST_END







