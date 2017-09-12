
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_reference)

TT_TEST_BEGIN(is_reference)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<int&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const int&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<volatile int &>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const volatile int &>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<r_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<cr_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<UDT&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const UDT&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<volatile UDT&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const volatile UDT&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<int (&)(int)>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<int (&)[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<int [2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const int [2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<volatile int [2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<const volatile int [2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<bool>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_reference<incomplete_type>::value, false);

TT_TEST_END







