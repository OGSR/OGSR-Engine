
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_type.hpp"
#include TYPE_TRANSFORM(remove_pointer)

BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_1, ::boost::remove_pointer, const, const)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_2, ::boost::remove_pointer, volatile, volatile)
BOOST_DECL_TRANSFORM_TEST3(remove_pointer_test_3, ::boost::remove_pointer, *)
BOOST_DECL_TRANSFORM_TEST0(remove_pointer_test_4, ::boost::remove_pointer)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_5, ::boost::remove_pointer, const &, const&)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_6, ::boost::remove_pointer, &, &)
BOOST_DECL_TRANSFORM_TEST3(remove_pointer_test_7, ::boost::remove_pointer, *volatile)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_8, ::boost::remove_pointer, const [2], const[2])
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_9, ::boost::remove_pointer, const &, const&)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_10, ::boost::remove_pointer, const*, const)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_11, ::boost::remove_pointer, volatile*, volatile)
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_12, ::boost::remove_pointer, const[2][3], const[2][3])
BOOST_DECL_TRANSFORM_TEST(remove_pointer_test_13, ::boost::remove_pointer, (&)[2], (&)[2])

TT_TEST_BEGIN(remove_pointer)

   remove_pointer_test_1();
   remove_pointer_test_2();
   remove_pointer_test_3();
   remove_pointer_test_4();
   remove_pointer_test_5();
   remove_pointer_test_6();
   remove_pointer_test_7();
   remove_pointer_test_8();
   remove_pointer_test_9();
   remove_pointer_test_10();
   remove_pointer_test_11();
   remove_pointer_test_12();
   remove_pointer_test_13();

TT_TEST_END







