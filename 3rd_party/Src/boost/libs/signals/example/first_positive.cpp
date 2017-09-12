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

#include <boost/signals/signal2.hpp>
#include <iostream>

template<typename T>
struct first_positive {
  typedef T result_type;

  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    while (first != last && !(*first > 0)) { ++first; }
    return (first == last) ? 0
                           : *first;
  }
};

template<typename T>
struct noisy_divide {
  typedef T result_type;

  T operator()(const T& x, const T& y) const
  {
    std::cout << "Dividing " << x << " and " << y << std::endl;
    return x/y;
  }
};

int main()
{
  boost::signal2<int, int, int, first_positive<int> > sig_positive;
  sig_positive.connect(std::plus<int>());
  sig_positive.connect(std::multiplies<int>());
  sig_positive.connect(std::minus<int>());
  sig_positive.connect(noisy_divide<int>());

  assert(sig_positive(3, -5) == 8); // returns 8, but prints nothing

  return 0;
}
