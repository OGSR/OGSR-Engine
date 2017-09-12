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
#include <cassert>

struct print_sum { 
  void operator()(int x, int y) const { std::cout << x+y << std::endl; } 
};

struct print_product {
  void operator()(int x, int y) const { std::cout << x*y << std::endl; }
};

struct print_difference { 
  void operator()(int x, int y) const { std::cout << x-y << std::endl; } 
};

struct print_quotient { 
  void operator()(int x, int y) const { std::cout << x/-y << std::endl; } 
};


int main()
{
  boost::signal2<void, int, int, boost::last_value<void>, std::string> sig;

  sig.connect(print_sum());
  sig.connect(print_product());

  sig(3, 5);

  boost::signals::connection print_diff_con = sig.connect(print_difference());

  // sig is still connected to print_diff_con
  assert(print_diff_con.connected());
  
  sig(5, 3); // prints 8, 15, and 2
  
  print_diff_con.disconnect(); // disconnect the print_difference slot
  
  sig(5, 3); // now prints 8 and 15, but not the difference
  
  assert(!print_diff_con.connected()); // not connected any more

  {
    boost::signals::scoped_connection c = sig.connect(print_quotient());
    sig(5, 3); // prints 8, 15, and 1
  } // c falls out of scope, so sig and print_quotient are disconnected
  
  sig(5, 3); // prints 8 and 15

  sig.connect("quotient", print_quotient());
  sig(5, 3); // prints 8, 15, and 1
  sig.disconnect("quotient");
  sig(5, 3); // prints 8 and 15

  return 0;
}
