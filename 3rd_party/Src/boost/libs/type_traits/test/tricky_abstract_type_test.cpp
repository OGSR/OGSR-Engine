
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_empty)
#include TYPE_TRAITS(is_stateless)

TT_TEST_BEGIN(tricky_abstract_type_test)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_stateless<test_abc1>::value, false);

TT_TEST_END







