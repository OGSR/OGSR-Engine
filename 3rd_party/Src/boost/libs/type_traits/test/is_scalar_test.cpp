
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(is_scalar)

TT_TEST_BEGIN(is_scalar)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<bool>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<bool const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<bool volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<bool const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<signed char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<signed char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<signed char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<signed char const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned char const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<char const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned short const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<short const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned short const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<short const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned int const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned int const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long const volatile>::value, true);

#ifdef BOOST_HAS_LONG_LONG

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned long long const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long long const volatile>::value, true);

#endif

#ifdef BOOST_HAS_MS_INT64

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int8>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int8>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int8 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int8 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int8 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int8 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int8 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int8 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int16>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int16>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int16 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int16 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int16 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int16 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int16 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int16 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int32>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int32>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int32 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int32 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int32 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int32 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int32 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int32 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int64>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int64>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int64 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int64 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int64 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int64 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<unsigned __int64 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<__int64 const volatile>::value, true);

#endif

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<float>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<float const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<float volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<float const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<double const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<double const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long double const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<long double const volatile>::value, true);


BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<void*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int*const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<f1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<f2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<f3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<mf1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<mf2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<mf3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<mp>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<cmf>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<enum_UDT>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_scalar<test_abc1>::value, false);

TT_TEST_END







