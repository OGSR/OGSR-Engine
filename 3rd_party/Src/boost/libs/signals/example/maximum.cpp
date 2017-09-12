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

#include <algorithm>
#include <iostream>
#include <boost/signals/signal2.hpp>

template<typename T>
struct maximum {
  typedef T result_type;

  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    if (first == last)
      throw std::runtime_error("Cannot compute maximum of zero elements!");
    return *std::max_element(first, last);
  }
};

int main()
{
  boost::signal2<int, int, int, maximum<int> > sig_max;
  sig_max.connect(std::plus<int>());
  sig_max.connect(std::multiplies<int>());
  sig_max.connect(std::minus<int>());
  sig_max.connect(std::divides<int>());

  std::cout << sig_max(5, 3) << std::endl; // prints 15
  
  return 0;
}
