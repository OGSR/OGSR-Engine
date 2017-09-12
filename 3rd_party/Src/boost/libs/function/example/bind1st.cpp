// Boost.Function library examples

// Copyright (C) 2001 Doug Gregor (gregod@cs.rpi.edu)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.

// For more information, see http://www.boost.org

#include <iostream>
#include <boost/function.hpp>
#include <functional>

struct X {
  X(int val) : value(val) {}

  int foo(int x) { return x * value; }

  int value;
};


int
main()
{
  boost::function<int, int> f;
  X x(7);
  f = std::bind1st(std::mem_fun(&X::foo), &x);

  std::cout << f(5) << std::endl; // Call x.foo(5)
  return 0;
}
