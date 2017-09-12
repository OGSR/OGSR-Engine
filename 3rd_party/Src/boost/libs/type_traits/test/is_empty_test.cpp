
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_empty)

struct non_default_constructable_UDT
{
   non_default_constructable_UDT(const non_default_constructable_UDT&){}
private:
   non_default_constructable_UDT(){}
};

TT_TEST_BEGIN(is_empty)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<int*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<f1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<mf1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<UDT>::value, false);

BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<empty_UDT>::value, true, false);
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<empty_POD_UDT>::value, true, false);
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<non_default_constructable_UDT>::value, true, false);
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<boost::noncopyable>::value, true, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<enum_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<non_empty>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<const non_empty&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_empty<foo4_t>::value, false);

TT_TEST_END







