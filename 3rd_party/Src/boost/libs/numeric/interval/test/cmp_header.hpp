/* Boost test/cmp_header.hpp header file
 *
 * Copyright Guillaume Melquiond 2003
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation.
 *
 * None of the above authors make any representation about the
 * suitability of this software for any purpose. It is provided "as
 * is" without express or implied warranty.
 *
 * $Id: cmp_header.hpp,v 1.2 2003/02/05 17:34:36 gmelquio Exp $
 */

#include <boost/numeric/interval/interval.hpp>
#include <boost/numeric/interval/checking.hpp>
#include <boost/numeric/interval/compare.hpp>
#include <boost/numeric/interval/policies.hpp>
#include <boost/test/test_tools.hpp>

struct empty_class {};

typedef boost::numeric::interval_lib::policies
          <empty_class, boost::numeric::interval_lib::checking_base<int> >
  my_policies;

typedef boost::numeric::interval<int, my_policies> I;

#define BOOST_C_EXN(e) \
  BOOST_CHECK_THROW(e, boost::numeric::interval_lib::comparison_error)
