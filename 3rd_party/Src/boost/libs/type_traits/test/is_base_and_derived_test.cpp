
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.


#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_base_and_derived)



TT_TEST_BEGIN(is_base_and_derived)

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Derived,Base>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Derived,Derived>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,Base>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,Derived>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,MultiBase>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Derived,MultiBase>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Derived2,MultiBase>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,PrivateBase>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<NonDerived,Base>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,void>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<Base,const void>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<void,Derived>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<const void,Derived>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<int, int>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<const int, int>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<VB,VD>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<VD,VB>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<test_abc1,test_abc3>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_base_and_derived<test_abc3,test_abc1>::value), false);

TT_TEST_END








