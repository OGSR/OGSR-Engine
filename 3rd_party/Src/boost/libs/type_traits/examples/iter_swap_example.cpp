
/*
 *
 * Copyright (c) 1999
 * Dr John Maddock
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Dr John Maddock makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * This file provides some example of type_traits usage -
 * by "optimising" various algorithms:
 *
 * opt::iter_swap - uses type_traits to determine whether the iterator is a proxy
 *                  in which case it uses a "safe" approach, otherwise calls swap
 *                  on the assumption that swap may be specialised for the pointed-to type.
 *
 */

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <iterator>
#include <vector>
#include <memory>

#include <boost/type_traits.hpp>

using std::cout;
using std::endl;
using std::cin;

namespace opt{

//
// iter_swap:
// tests whether iterator is a proxying iterator or not, and
// uses optimal form accordingly:
//
namespace detail{

template <bool b>
struct swapper
{
   template <typename I>
   static void do_swap(I one, I two)
   {
      typedef typename std::iterator_traits<I>::value_type v_t;
      v_t v = *one;
      *one = *two;
      *two = v;
   }
};

#ifdef __GNUC__
using std::swap;
#endif

template <>
struct swapper<true>
{
   template <typename I>
   static void do_swap(I one, I two)
   {
      using std::swap;
      swap(*one, *two);
   }
};

}

template <typename I1, typename I2>
inline void iter_swap(I1 one, I2 two)
{
   typedef typename std::iterator_traits<I1>::reference r1_t;
   typedef typename std::iterator_traits<I2>::reference r2_t;
   detail::swapper<
      ::boost::type_traits::ice_and<
         ::boost::is_reference<r1_t>::value, 
         ::boost::is_reference<r2_t>::value,
         ::boost::is_same<r1_t, r2_t>::value
      >::value>::do_swap(one, two);
}


};   // namespace opt

int cpp_main(int argc, char* argv[])
{
   //
   // testing iter_swap
   // really just a check that it does in fact compile...
   std::vector<int> v1;
   v1.push_back(0);
   v1.push_back(1);
   std::vector<bool> v2;
   v2.push_back(0);
   v2.push_back(1);
   opt::iter_swap(v1.begin(), v1.begin()+1);
   opt::iter_swap(v2.begin(), v2.begin()+1);

   return 0;
}


