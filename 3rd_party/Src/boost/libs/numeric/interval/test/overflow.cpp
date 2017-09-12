/* Boost test/overflow.cpp
 * test if extended precision exponent does not disturb interval computation
 *
 * Copyright Guillaume Melquiond 2002-2003
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation.
 *
 * None of the above authors nor Polytechnic University make any
 * representation about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * $Id: overflow.cpp,v 1.3 2003/02/05 17:34:36 gmelquio Exp $
 */

#include <boost/numeric/interval.hpp>
#include <boost/test/minimal.hpp>

template<class I>
void test_one(typename I::base_type x, typename I::base_type f) {
  I y = x;
  typename I::base_type g = 1 / f;
  const int nb = 10000;
  for(int i = 0; i < nb; i++) y *= f;
  for(int i = 0; i < nb; i++) y *= g;
  BOOST_TEST(in(x, y));
}

template<class I>
void test() {
  test_one<I>(1., 25.);
  test_one<I>(1., 0.04);
  test_one<I>(-1., 25.);
  test_one<I>(-1., 0.04);
}

int test_main(int, char *[]) {
  test<boost::numeric::interval<float> >();
  test<boost::numeric::interval<double> >();
  test<boost::numeric::interval<long double> >();
  return 0;
}
