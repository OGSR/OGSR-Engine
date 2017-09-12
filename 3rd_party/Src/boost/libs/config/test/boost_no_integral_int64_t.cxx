//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_INTEGRAL_INT64_T
//  TITLE:         long long and integral constant expressions
//  DESCRIPTION:   The platform supports long long in integral constant expressions.

#include <cstdlib>


namespace boost_no_integral_int64_t{

#ifdef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
enum{ mask = 1uLL << 50 };

template <unsigned long long m>
struct llt
{
   enum{ value = m };
};
#else
static const unsigned long long mask = 1uLL << 50;

template <unsigned long long m>
struct llt
{
   static const unsigned long long value = m;
};
#endif

int test()
{
   llt<mask> m;
   (void)m;
   return 0;
}

}




