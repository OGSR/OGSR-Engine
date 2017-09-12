
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_compound)

TT_TEST_BEGIN(is_compound)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<void*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<test_abc1>::value, true);

TT_TEST_END







