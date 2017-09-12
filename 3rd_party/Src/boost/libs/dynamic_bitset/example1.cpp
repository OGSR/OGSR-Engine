// (C) Copyright Jeremy Siek 2001. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

// Sample output:
//  1
//  1
//  0
//  0
//  1

#include <iostream>
#include <boost/dynamic_bitset.hpp>
int main() {
  boost::dynamic_bitset<> x(5); // all 0's by default
  x[0] = 1;
  x[1] = 1;
  x[4] = 1;
  for (boost::dynamic_bitset<>::size_type i = 0; i < x.size(); ++i)
    std::cout << x[i];
  std::cout << "\n";
  std::cout << x << "\n";
  return EXIT_SUCCESS;
}
