/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/constrained_value.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include <iostream>

class bad_day {}; //exception type


class day_value_policies
{
public:
  typedef unsigned int value_type;
  static unsigned int min() { return 0; };
  static unsigned int max() { return 31;};
  static void on_error(unsigned int&, unsigned int, boost::CV::violation_enum)
  {
    throw bad_day();
  }
};

struct range_error {}; //exception type
typedef boost::CV::simple_exception_policy<int,1,10,range_error> one_to_ten_range_policy;

int main()
{
  using namespace boost::CV;
  constrained_value<day_value_policies> cv1(0), cv2(31);
  check("not equal", cv1 != cv2);
  check("equal", cv1 == cv1);
  check("greater", cv2 > cv1);
  check("greater or equal ", cv2 >= cv1);
  //various running of the conversion operator
  std::cout << cv1 << std::endl;
  unsigned int i = cv1; 
  check("test conversion", i == cv1);


  try {
    constrained_value<one_to_ten_range_policy> cv2(11);
    std::cout << "Not Reachable: " << cv2 << " ";
    check("got range exception max", false);
  }
  catch(range_error& e) {
    check("got range exception max", true);
  }

  try {
    constrained_value<one_to_ten_range_policy> cv3(0);
    std::cout << "Not Reachable: " << cv3 << " ";
    check("got range exception min", false);
  }
  catch(range_error& e) {
    check("got range exception min", true);
  }

  try {
    constrained_value<one_to_ten_range_policy> cv4(1);
    cv4 = 12;
    check("range exception on assign", false);
  }
  catch(range_error& e) {
    check("range exception on assign", true);
  }

  return printTestStats();
}

/*
 * Copyright (c) 2001
 * CrystalClear Software, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  CrystalClear Software makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided as is without express or implied warranty.
 *
 *
 * Author:  Jeff Garland (jeff@CrystalClearSoftware.com)
 *
 */
