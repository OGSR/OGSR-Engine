
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_type.hpp"
#include TYPE_TRANSFORM(add_pointer)

BOOST_DECL_TRANSFORM_TEST(add_pointer_test_1, ::boost::add_pointer, const, const*)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_2, ::boost::add_pointer, volatile, volatile*)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_3, ::boost::add_pointer, *, **)
BOOST_DECL_TRANSFORM_TEST2(add_pointer_test_4, ::boost::add_pointer, *)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_7, ::boost::add_pointer, *volatile, *volatile*)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_10, ::boost::add_pointer, const*, const**)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_11, ::boost::add_pointer, volatile*, volatile**)

TT_TEST_BEGIN(add_pointer)

   add_pointer_test_1();
   add_pointer_test_2();
   add_pointer_test_3();
   add_pointer_test_4();
   add_pointer_test_7();
   add_pointer_test_10();
   add_pointer_test_11();

TT_TEST_END







