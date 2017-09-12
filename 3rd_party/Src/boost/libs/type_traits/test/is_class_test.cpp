
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_class)

TT_TEST_BEGIN(is_class)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<const int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<volatile int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<int*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<int* const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<mf4>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<f1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<enum_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<union_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<POD_union_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<empty_union_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<empty_POD_union_UDT>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<empty_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<std::iostream>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<test_abc1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<test_abc1 const>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<UDT&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<void>::value, false);

TT_TEST_END







