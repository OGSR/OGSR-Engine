/* Boost test/pow.cpp
 * test the pow function
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
 * $Id: pow.cpp,v 1.3 2003/02/05 17:34:36 gmelquio Exp $
 */

#include <boost/numeric/interval.hpp>
#include <boost/test/minimal.hpp>

bool test_pow(double al, double au, double bl, double bu, int p) {
  typedef boost::numeric::interval<double> I;
  I b = pow(I(al, au), p);
  return b.lower() == bl && b.upper() == bu;
}

int test_main(int, char *[]) {
  BOOST_TEST(test_pow(2, 3, 8, 27, 3));
  BOOST_TEST(test_pow(2, 3, 16, 81, 4));
  BOOST_TEST(test_pow(-3, 2, -27, 8, 3));
  BOOST_TEST(test_pow(-3, 2, 0, 81, 4));
  BOOST_TEST(test_pow(-3, -2, -27, -8, 3));
  BOOST_TEST(test_pow(-3, -2, 16, 81, 4));

  BOOST_TEST(test_pow(2, 4, 1./64, 1./8, -3));
  BOOST_TEST(test_pow(2, 4, 1./256, 1./16, -4));
  BOOST_TEST(test_pow(-4, -2, -1./8, -1./64, -3));
  BOOST_TEST(test_pow(-4, -2, 1./256, 1./16, -4));

  BOOST_TEST(test_pow(2, 3, 1, 1, 0));
  BOOST_TEST(test_pow(-3, 2, 1, 1, 0));
  BOOST_TEST(test_pow(-3, -2, 1, 1, 0));

  return 0;
}
