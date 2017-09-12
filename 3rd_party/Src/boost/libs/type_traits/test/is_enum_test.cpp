
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_enum)

TT_TEST_BEGIN(is_enum)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<enum_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<int_convertible>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<boost::noncopyable>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<test_abc1>::value, false);

TT_TEST_END







