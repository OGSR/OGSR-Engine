
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_function)
#include TYPE_TRAITS(is_float)
#include TYPE_TRAITS(is_enum)
#include TYPE_TRAITS(is_class)
#include TYPE_TRAITS(is_scalar)
#include TYPE_TRAITS(is_pod)
#include TYPE_TRAITS(has_trivial_constructor)
#include TYPE_TRAITS(has_trivial_copy)
#include TYPE_TRAITS(has_trivial_assign)
#include TYPE_TRAITS(has_trivial_destructor)
#include TYPE_TRAITS(is_compound)
#include TYPE_TRAITS(is_base_and_derived)

TT_TEST_BEGIN(tricky_function_type_test)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<const int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_function<int (&)(int)>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<foo0_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<foo0_t, foo0_t>::value), false);


BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<foo1_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<foo1_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<foo1_t>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<foo2_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<foo2_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<foo2_t>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<foo3_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<foo3_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<foo3_t>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<foo4_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<foo4_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<foo4_t>::value, false);

TT_TEST_END







