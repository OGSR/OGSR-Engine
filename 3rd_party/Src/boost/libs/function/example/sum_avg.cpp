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

void do_sum_avg(int values[], int n, int& sum, float& avg)
{
  sum = 0;
  for (int i = 0; i < n; i++)
    sum += values[i];
  avg = (float)sum / n;
}

int
main()
{
  // The second parameter should be int[], but some compilers (e.g., GCC)
  // complain about this 
  boost::function<void, int*, int, int&, float&> sum_avg;

  sum_avg = &do_sum_avg;

  int values[5] = { 1, 1, 2, 3, 5 };
  int sum;
  float avg;
  sum_avg(values, 5, sum, avg);

  std::cout << "sum = " << sum << std::endl;
  std::cout << "avg = " << avg << std::endl;
  return 0;
}
