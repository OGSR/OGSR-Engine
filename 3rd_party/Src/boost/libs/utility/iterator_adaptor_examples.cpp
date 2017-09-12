// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.


#include <functional>
#include <algorithm>
#include <iostream>
#include <boost/iterator_adaptors.hpp>
#include <boost/pending/integer_range.hpp>

int
main(int, char*[])
{
  // This is a simple example of using the transform_iterators class to
  // generate iterators that multiply the value returned by dereferencing
  // the iterator. In this case we are multiplying by 2.
  // Would be cooler to use lambda library in this example.

  int x[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

  typedef std::binder1st< std::multiplies<int> > Function;
  typedef boost::transform_iterator_generator<Function, int* 
  >::type doubling_iterator;

  doubling_iterator i(x, std::bind1st(std::multiplies<int>(), 2)),
    i_end(x + sizeof(x)/sizeof(int), std::bind1st(std::multiplies<int>(), 2));

  std::cout << "multiplying the array by 2:" << std::endl;
  while (i != i_end)
    std::cout << *i++ << " ";
  std::cout << std::endl;

  // Here is an example of counting from 0 to 5 using the integer_range class.

  boost::integer_range<int> r(0,5);

  std::cout << "counting to from 0 to 4:" << std::endl;
  std::copy(r.begin(), r.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  return 0;
}


