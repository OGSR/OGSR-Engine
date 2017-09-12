
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
 * opt::fill - optimised for trivial copy/small types (cf std::fill)
 *
 */

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <iterator>
#include <memory>
#include <cstring>

#include <boost/timer.hpp>
#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE) || (defined(std) && defined(__SGI_STL_PORT))
namespace std{ using :: memset; }
#endif

using std::cout;
using std::endl;
using std::cin;

namespace opt{
//
// fill
// same as std::fill, uses memset where appropriate, along with call_traits
// to "optimise" parameter passing.
//
namespace detail{

template <typename I, typename T>
void do_fill_(I first, I last, typename boost::call_traits<T>::param_type val)
{
   while(first != last)
   {
      *first = val;
      ++first;
   }
}

template <bool opt>
struct filler
{
   template <typename I, typename T>
   struct rebind
   {
      static void do_fill(I first, I last, typename boost::call_traits<T>::param_type val)
      { do_fill_<I,T>(first, last, val); }
   };
};

template <>
struct filler<true>
{
   template <typename I, typename T>
   struct rebind
   {
      static void do_fill(I first, I last, T val)
      {
         std::memset(first, val, last-first);
      }
   };
};

}

template <class I, class T>
inline void fill(I first, I last, const T& val)
{
   typedef detail::filler<
      ::boost::type_traits::ice_and<
         ::boost::is_pointer<I>::value,
         ::boost::is_arithmetic<T>::value,
         (sizeof(T) == 1)
      >::value> filler_t;
   typedef typename filler_t:: template rebind<I,T> binder;
   binder::do_fill(first, last, val);
}

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
   //
   // test destroy_array,
   // compare destruction time of an array of ints
   // with unoptimised form.
   //
   cout << "Measuring times in micro-seconds per 1000 elements processed" << endl << endl;

   cout << "testing fill(char)...\n"
   "[Some standard library versions may already perform this optimisation.]" << endl;

   // cache load:
   opt::fill(c_array, c_array + array_size, (char)3);

   // time optimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::fill(c_array, c_array + array_size, (char)3);
   }
   result = t.elapsed();
   cout << "opt::fill<char*, char>: " << result << endl;

   // cache load:
   std::fill(c_array, c_array + array_size, (char)3);

   // time standard version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      std::fill(c_array, c_array + array_size, (char)3);
   }
   result = t.elapsed();
   cout << "std::fill<char*, char>: " << result << endl << endl;

   cout << "testing fill(int)...\n"
   "[Tests the effect of call_traits pass-by-value optimisation -\nthe value of this optimisation may depend upon hardware characteristics.]" << endl;

   // cache load:
   opt::fill(i_array, i_array + array_size, 3);

   // timer optimised version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      opt::fill(i_array, i_array + array_size, 3);
   }
   result = t.elapsed();
   cout << "opt::fill<int*, int>: " << result << endl;

   // cache load:
   std::fill(i_array, i_array + array_size, 3);

   // time standard version:
   t.restart();
   for(i = 0; i < iter_count; ++i)
   {
      std::fill(i_array, i_array + array_size, 3);
   }
   result = t.elapsed();
   cout << "std::fill<int*, int>: " << result << endl << endl;

   return 0;
}





