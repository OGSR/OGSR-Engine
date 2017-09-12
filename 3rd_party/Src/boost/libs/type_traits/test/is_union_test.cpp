
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_union)

TT_TEST_BEGIN(is_union)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<const int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<volatile int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<int*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<int* const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<mf4>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<f1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<enum_UDT>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<POD_union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<empty_union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<empty_POD_union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<union_UDT const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<POD_union_UDT volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<empty_union_UDT const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<empty_POD_union_UDT const>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<empty_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<std::iostream>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<UDT&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<void>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_union<incomplete_type>::value, false);

TT_TEST_END








