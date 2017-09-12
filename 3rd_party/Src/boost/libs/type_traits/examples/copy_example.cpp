
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
 * opt::copy - optimised for trivial copy (cf std::copy)
 *
 */

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <iterator>
#include <memory>

#include <boost/timer.hpp>
#include <boost/type_traits.hpp>

using std::cout;
using std::endl;
using std::cin;

namespace opt{

//
// opt::copy
// same semantics as std::copy
// calls memcpy where appropiate.
//

namespace detail{

template<typename I1, typename I2>
I2 copy_imp(I1 first, I1 last, I2 out)
{
   while(first != last)
   {
      *out = *first;
      ++out;
      ++first;
   }
   return out;
}

template <bool b>
struct copier
{
   template<typename I1, typename I2>
   static I2 do_copy(I1 first, I1 last, I2 out)
   { return copy_imp(first, last, out); }
};

template <>
struct copier<true>
{
   template<typename I1, typename I2>
   static I2* do_copy(I1* first, I1* last, I2* out)
   {
      memcpy(out, first, (last-first)*sizeof(I2));
      return out+(last-first);
   }
};


}

#ifndef BOOST_NO_STD_ITERATOR_TRAITS

template<typename I1, typename I2>
inline I2 copy(I1 first, I1 last, I2 out)
{
   typedef typename boost::remove_cv<typename std::iterator_traits<I1>::value_type>::type v1_t;
   typedef typename boost::remove_cv<typename std::iterator_traits<I2>::value_type>::type v2_t;
   return detail::copier<
      ::boost::type_traits::ice_and<
         ::boost::is_same<v1_t, v2_t>::value,
         ::boost::is_pointer<I1>::value,
         ::boost::is_pointer<I2>::value,
         ::boost::has_trivial_assign<v1_t>::value
      >::value>::do_copy(first, last, out);
}

#else // BOOST_NO_STD_ITERATOR_TRAITS

//
// If there is no standard iterator_traits then we have to
// use overloading rather than iterator_traits to detect
// when we have T*'s to copy.  Note that we cannot overload
// copy directly as that will cause some standard conforming
// code to fail to build:

namespace detail{

template<typename I1, typename I2>
inline I2 copy_(const I1& first, const I1& last, const I2& out)
{
   return detail::copier<false>::do_copy(first, last, out);
}

template<typename T>
inline T* copy_(const T*& first, const T*& last, T*& out)
{
   return detail::copier< 
      ::boost::has_trivial_assign<T>::value
   >::do_copy(first, last, out);
}

} // namespace detail

template<typename I1, typename I2>
inline I2 copy(I1 first, I1 last, I2 out)
{
   return detail::copy_(first, last, out);
}

#endif // BOOST_NO_STD_ITERATOR_TRAITS

};   // namespace opt

//
// define some global data:
//
const int array_size = 1000;
int i_array_[array_size] = {0,};
const int ci_array_[array_size] = {0,};
char c_array_[array_size] = {0,};
const char cc_array_[array_size] = { 0,};
//
// since arrays aren't iterators we define a set of pointer
// aliases into the arrays (otherwise the compiler is entitled
// to deduce the type passed to the template functions as
// T (&)[N] rather than T*).
int* i_array = i_array_;
const int* ci_array = ci_array_;
char* c_array = c_array_;
const char* cc_array = cc_array_;

const int iter_count = 1000000;

int cpp_main(int argc, char* argv[])
{
   boost::timer t;
   double result;
   int i;
   cout << "Measuring times in micro-seconds per 1000 elements processed" << endl << endl;
   cout << "testing copy...\n"
   "[Some standard library versions may already perform this optimisation.]" << endl;
   
   // cache load:
   opt::copy(ci_array, ci_array + array_size, i_array);

   // time optimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::copy(ci_array, ci_array + array_size, i_array);
   }
   result = t.elapsed();
   cout << "opt::copy<const int*, int*>: " << result << endl;

   // cache load:
   std::copy(ci_array, ci_array + array_size, i_array);

   // time standard version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      std::copy(ci_array, ci_array + array_size, i_array);
   }
   result = t.elapsed();
   cout << "std::copy<const int*, int*>: " << result << endl;

   // cache load:
   opt::detail::copier<false>::do_copy(ci_array, ci_array + array_size, i_array);

   // time unoptimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::detail::copier<false>::do_copy(ci_array, ci_array + array_size, i_array);
   }
   result = t.elapsed();
   cout << "standard \"unoptimised\" copy: " << result << endl << endl;

   // cache load:
   opt::copy(cc_array, cc_array + array_size, c_array);

   // time optimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::copy(cc_array, cc_array + array_size, c_array);
   }
   result = t.elapsed();
   cout << "opt::copy<const char*, char*>: " << result << endl;

   // cache load:
   std::copy(cc_array, cc_array + array_size, c_array);

   // time standard version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      std::copy(cc_array, cc_array + array_size, c_array);
   }
   result = t.elapsed();
   cout << "std::copy<const char*, char*>: " << result << endl;

   // cache load:
   opt::detail::copier<false>::do_copy(cc_array, cc_array + array_size, c_array);

   // time unoptimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::detail::copier<false>::do_copy(cc_array, cc_array + array_size, c_array);
   }
   result = t.elapsed();
   cout << "standard \"unoptimised\" copy: " << result << endl << endl;

   return 0;
}







