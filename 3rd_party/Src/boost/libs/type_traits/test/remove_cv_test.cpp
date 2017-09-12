
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_type.hpp"
#include TYPE_TRANSFORM(remove_cv)

BOOST_DECL_TRANSFORM_TEST3(remove_cv_test_1, ::boost::remove_cv, const)
BOOST_DECL_TRANSFORM_TEST3(remove_cv_test_2, ::boost::remove_cv, volatile)
BOOST_DECL_TRANSFORM_TEST3(remove_cv_test_3, ::boost::remove_cv, const volatile)
BOOST_DECL_TRANSFORM_TEST0(remove_cv_test_4, ::boost::remove_cv)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_5, ::boost::remove_cv, const &, const&)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_6, ::boost::remove_cv, *const, *)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_7, ::boost::remove_cv, *volatile, *)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_8, ::boost::remove_cv, *const volatile, *)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_9, ::boost::remove_cv, *, *)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_10, ::boost::remove_cv, const*, const*)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_11, ::boost::remove_cv, volatile*, volatile*)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_12, ::boost::remove_cv, const[2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_13, ::boost::remove_cv, volatile[2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_14, ::boost::remove_cv, const volatile[2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_15, ::boost::remove_cv, [2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_16, ::boost::remove_cv, const*, const*)
BOOST_DECL_TRANSFORM_TEST(remove_cv_test_17, ::boost::remove_cv, const*volatile, const*)

TT_TEST_BEGIN(remove_cv)

   remove_cv_test_1();
   remove_cv_test_2();
   remove_cv_test_3();
   remove_cv_test_4();
   remove_cv_test_5();
   remove_cv_test_6();
   remove_cv_test_7();
   remove_cv_test_8();
   remove_cv_test_9();
   remove_cv_test_10();
   remove_cv_test_11();
   remove_cv_test_12();
   remove_cv_test_13();
   remove_cv_test_14();
   remove_cv_test_15();
   remove_cv_test_16();
   remove_cv_test_17();

TT_TEST_END







