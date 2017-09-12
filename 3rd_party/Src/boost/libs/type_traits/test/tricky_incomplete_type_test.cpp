
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include <boost/type_traits.hpp>

TT_TEST_BEGIN(tricky_incomplete_type_test)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_nothrow_assign<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_nothrow_constructor<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_nothrow_copy<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_assign<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_constructor<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_copy<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_trivial_destructor<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_class<incomplete_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_compound<incomplete_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_enum<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_pod<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<incomplete_type>::value, false);

TT_TEST_END

