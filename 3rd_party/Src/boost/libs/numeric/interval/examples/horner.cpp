/* Boost example/horner.cpp
 * example of unprotecting rounding for a whole function computation
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
 * $Id: horner.cpp,v 1.2 2003/02/05 17:34:34 gmelquio Exp $
 */

#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/io.hpp>
#include <iostream>

// I is an interval class, the polynom is a simple array
template<class I>
I horner(const I& x, const I p[], int n) {

  // initialize and restore the rounding mode
  typename I::traits_type::rounding rnd;

  // define the unprotected version of the interval type
  typedef typename boost::numeric::interval_lib::unprotect<I>::type R;

  const R& a = x;
  R y = p[n - 1];
  for(int i = n - 2; i >= 0; i--) {
    y = y * a + (const R&)(p[i]);
  }
  return y;

  // restore the rounding mode with the destruction of rnd
}

int main() {
  typedef boost::numeric::interval<double> I;
  I p[3] = { -1.0, 0, 1.0 };
  I x = 1.0;
  std::cout << horner(x, p, 3) << std::endl;
  return 0;
}
