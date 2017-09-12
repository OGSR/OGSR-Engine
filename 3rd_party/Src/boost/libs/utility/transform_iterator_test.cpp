//  (C) Copyright Jeremy Siek 1999. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

//  Revision History
//  08 Mar 2001   Jeremy Siek
//       Moved test of transform iterator into its own file. It to
//       to be in iterator_adaptor_test.cpp.

#include <boost/config.hpp>
#include <iostream>
#include <algorithm>
#include <boost/iterator_adaptors.hpp>
#include <boost/pending/iterator_tests.hpp>

struct mult_functor {
  typedef int result_type;
  typedef int argument_type;
  // Functors used with transform_iterator must be
  // DefaultConstructible, as the transform_iterator must be
  // DefaultConstructible to satisfy the requirements for
  // TrivialIterator.
  mult_functor() { }
  mult_functor(int aa) : a(aa) { }
  int operator()(int b) const { return a * b; }
  int a;
};

int
main()
{
  const int N = 10;

  // Borland is getting confused about typedef's and constructors here

  // Test transform_iterator
  {
    int x[N], y[N];
    for (int k = 0; k < N; ++k)
      x[k] = k;
    std::copy(x, x + N, y);

    for (int k2 = 0; k2 < N; ++k2)
      x[k2] = x[k2] * 2;

    boost::transform_iterator_generator<mult_functor, int*>::type i(y, mult_functor(2));
    boost::input_iterator_test(i, x[0], x[1]);
    boost::input_iterator_test(boost::make_transform_iterator(&y[0], mult_functor(2)), x[0], x[1]);
  }
  std::cout << "test successful " << std::endl;
  return 0;
}
