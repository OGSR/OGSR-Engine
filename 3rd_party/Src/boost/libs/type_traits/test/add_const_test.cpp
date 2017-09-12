
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_type.hpp"
#include TYPE_TRANSFORM(add_const)

BOOST_DECL_TRANSFORM_TEST(add_const_test_1, ::boost::add_const, const, const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_2, ::boost::add_const, volatile, volatile const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_3, ::boost::add_const, *, *const)
BOOST_DECL_TRANSFORM_TEST2(add_const_test_4, ::boost::add_const, const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_7, ::boost::add_const, *volatile, *volatile const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_10, ::boost::add_const, const*, const*const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_11, ::boost::add_const, volatile*, volatile*const)
BOOST_DECL_TRANSFORM_TEST(add_const_test_5, ::boost::add_const, const &, const&)
BOOST_DECL_TRANSFORM_TEST(add_const_test_6, ::boost::add_const, &, &)
BOOST_DECL_TRANSFORM_TEST(add_const_test_8, ::boost::add_const, const [2], const [2])
BOOST_DECL_TRANSFORM_TEST(add_const_test_9, ::boost::add_const, volatile &, volatile&)
BOOST_DECL_TRANSFORM_TEST(add_const_test_12, ::boost::add_const, [2][3], const[2][3])
BOOST_DECL_TRANSFORM_TEST(add_const_test_13, ::boost::add_const, (&)[2], (&)[2])

TT_TEST_BEGIN(add_const)

   add_const_test_1();
   add_const_test_2();
   add_const_test_3();
   add_const_test_4();
   add_const_test_7();
   add_const_test_10();
   add_const_test_11();
   add_const_test_5();
   add_const_test_6();
   add_const_test_8();
   add_const_test_9();
   add_const_test_12();
   add_const_test_13();

TT_TEST_END







