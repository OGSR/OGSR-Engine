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
#include <string>
#include <functional>

void print_sum(int x, int y)
{
  std::cout << "Sum = " << x+y << std::endl;
}

void print_product(int x, int y)
{
  std::cout << "Product = " << x*y << std::endl;
}

void print_quotient(float x, float y)
{
  std::cout << "Quotient = " << x/y << std::endl;
}

int main()
{
  typedef boost::signal2<void, int, int, boost::last_value<void>,
                         std::string, std::less<std::string>,
                         void (*)(int, int)> sig_type;

  sig_type sig;
  sig.connect(&print_sum);
  sig.connect(&print_product);

  sig(3, 5); // print sum and product of 3 and 5

  // should fail
  //   sig.connect(&print_quotient);
}
