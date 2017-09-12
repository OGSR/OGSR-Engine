// Boost.Signals library
//
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
#include <boost/signals/signal2.hpp>

struct print_sum { 
  void operator()(int x, int y) const { std::cout << x+y << std::endl; } 
};

struct print_product {
  void operator()(int x, int y) const { std::cout << x*y << std::endl; }
};


int main()
{
  boost::signal2<void, int, int> sig;

  sig.connect(print_sum());
  sig.connect(print_product());

  sig(3, 5); // prints 8 and 15

  return 0;
}
