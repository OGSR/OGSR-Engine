
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_pointer)

TT_TEST_BEGIN(is_pointer)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<const int*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<volatile int*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<non_pointer*>::value, true);
   // these were false in previous versions (JM 20 Dec 2000):
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int*const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int*volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int*const volatile>::value, true);
   // JM 02 Oct 2000:
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<non_pointer>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int*&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int(&)[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<char[sizeof(void*)]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<void>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<f1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<f2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<f3>::value, true);
   // Steve: was 'true', should be 'false', via 3.9.2p3
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<mf1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<mf2>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<mf3>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<mf4>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<test_abc1>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<foo4_t>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pointer<test_abc1>::value, false);

TT_TEST_END







