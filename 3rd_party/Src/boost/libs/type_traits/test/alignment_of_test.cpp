
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "test.hpp"
#include "check_integral_constant.hpp"
#include TYPE_TRAITS(alignment_of)

//
// VC++ emits an awful lot of warnings unless we define these:
#ifdef BOOST_MSVC
#  pragma warning(disable:4244)
#endif


template <class T>
struct align_calc
{
   char padding;
   T instance;
   static std::ptrdiff_t get()
   {
      static align_calc<T> a;
      return reinterpret_cast<const char*>(&(a.instance)) - reinterpret_cast<const char*>(&(a.padding));
   }
};

#define ALIGNOF(x) align_calc<x>::get()

TT_TEST_BEGIN(alignment_of)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<void>::value, 0);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<char>::value, ALIGNOF(char));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<short>::value, ALIGNOF(short));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<int>::value, ALIGNOF(int));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<long>::value, ALIGNOF(long));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<float>::value, ALIGNOF(float));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<double>::value, ALIGNOF(double));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<long double>::value, ALIGNOF(long double));
#ifdef BOOST_HAS_LONG_LONG
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<long long>::value, ALIGNOF(long long));
#endif
#ifdef BOOST_HAS_MS_INT64
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<__int64>::value, ALIGNOF(__int64));
#endif
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<int[4]>::value, ALIGNOF(int[4]));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<int(*)(int)>::value, ALIGNOF(int(*)(int)));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<int*>::value, ALIGNOF(int*));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<VB>::value, ALIGNOF(VB));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<VD>::value, ALIGNOF(VD));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<enum_UDT>::value, ALIGNOF(enum_UDT));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<mf2>::value, ALIGNOF(mf2));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<POD_UDT>::value, ALIGNOF(POD_UDT));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<empty_UDT>::value, ALIGNOF(empty_UDT));
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::alignment_of<union_UDT>::value, ALIGNOF(union_UDT));

TT_TEST_END







