// -- control_structures.cpp -- The Boost Lambda Library ------------------
//
// Copyright (C) 2000-2003 Jaakko Järvi (jaakko.jarvi@cs.utu.fi)
// Copyright (C) 2000-2003 Gary Powell (powellg@amazon.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies. 
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice 
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty, 
// and with no claim as to its suitability for any purpose.
//
// For more information, see www.boost.org

// -----------------------------------------------------------------------

#define BOOST_INCLUDE_MAIN  // for testing, include rather than link
#include <boost/test/test_tools.hpp>    // see "Header Implementation Option"

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"
#include "boost/lambda/loops.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

using namespace boost;
using namespace boost::lambda;

// 2 container for_each
template <class InputIter1, class InputIter2, class Function>
Function for_each(InputIter1 first, InputIter1 last, 
                  InputIter2 first2, Function f) {
  for ( ; first != last; ++first, ++first2)
    f(*first, *first2);
  return f;
}

void simple_loops() {

  // for loops ---------------------------------------------------------
  int i;
  int arithmetic_series = 0;
  for_loop(_1 = 0, _1 < 10, _1++, arithmetic_series += _1)(i);
  BOOST_TEST(arithmetic_series == 45);

  // no body case
  for_loop(var(i) = 0, var(i) < 100, ++var(i))();
  BOOST_TEST(i == 100);
 
  // while loops -------------------------------------------------------
  int a = 0, b = 0, c = 0;

  while_loop((_1 + _2) >= (_1 * _2), (++_1, ++_2, ++_3))(a, b, c); 
  BOOST_TEST(c == 3);

  int count;
  count = 0; i = 0;
  while_loop(_1++ < 10, ++var(count))(i);
  BOOST_TEST(count == 10);

  // note that the first parameter of do_while_loop is the condition
  count = 0; i = 0;
  do_while_loop(_1++ < 10, ++var(count))(i);
  BOOST_TEST(count == 11);

  a = 0;
  do_while_loop(constant(false), _1++)(a);
  BOOST_TEST(a == 1);
 
  // no body cases
  a = 40; b = 30;
  while_loop(--_1 > _2)(a, b);
  BOOST_TEST(a == b);

  // (the no body case for do_while_loop is pretty redundant)
  a = 40; b = 30;
  do_while_loop(--_1 > _2)(a, b);
  BOOST_TEST(a == b);


}

void simple_ifs () {

  int value = 42;
  if_then(_1 < 0, _1 = 0)(value);
  BOOST_TEST(value == 42);

  value = -42;
  if_then(_1 < 0, _1 = -_1)(value);
  BOOST_TEST(value == 42);

  int min;
  if_then_else(_1 < _2, var(min) = _1, var(min) = _2)
     (make_const(1), make_const(2));
  BOOST_TEST(min == 1);

  if_then_else(_1 < _2, var(min) = _1, var(min) = _2)
     (make_const(5), make_const(3));
  BOOST_TEST(min == 3);

  int x, y;
  x = -1; y = 1;
  BOOST_TEST(if_then_else_return(_1 < _2, _2, _1)(x, y) == std::max(x ,y));
  BOOST_TEST(if_then_else_return(_1 < _2, _2, _1)(y, x) == std::max(x ,y));
}


int test_main(int, char *[]) 
{
  simple_loops();
  simple_ifs();
  return 0;
}
