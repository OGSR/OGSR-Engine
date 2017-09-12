
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_type.hpp"
#include TYPE_TRANSFORM(add_pointer)

BOOST_DECL_TRANSFORM_TEST(add_pointer_test_5, ::boost::add_pointer, const &, const*)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_6, ::boost::add_pointer, &, *)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_8, ::boost::add_pointer, const [2], const (*)[2])
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_9, ::boost::add_pointer, const &, const*)
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_12, ::boost::add_pointer, const[2][3], const (*)[2][3])
BOOST_DECL_TRANSFORM_TEST(add_pointer_test_13, ::boost::add_pointer, (&)[2], (*)[2])


TT_TEST_BEGIN(tricky_add_pointer_test)

   add_pointer_test_5();
   add_pointer_test_6();
   add_pointer_test_8();
   add_pointer_test_9();
   add_pointer_test_12();
   add_pointer_test_13();

TT_TEST_END







