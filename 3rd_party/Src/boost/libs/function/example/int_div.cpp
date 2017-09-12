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

struct int_div { 
  float operator()(int x, int y) const { return ((float)x)/y; }; 
};

int
main()
{
  boost::function<float, int, int> f;
  f = int_div();

  std::cout << f(5, 3) << std::endl; // 1.66667

  return 0;
}
