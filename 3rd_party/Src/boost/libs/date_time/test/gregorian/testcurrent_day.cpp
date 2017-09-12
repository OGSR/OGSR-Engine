/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>

int
main() 
{

  boost::gregorian::date d1(boost::gregorian::day_clock::local_day());
  std::cout << "Check the printed date by hand: "
            <<  boost::gregorian::to_iso_string(d1) << std::endl;

  using namespace boost::gregorian;
  date::ymd_type ymd = day_clock::local_day_ymd();
  std::cout << ymd.year << "-" 
            << ymd.month.as_long_string() << "-"
            << ymd.day << std::endl;

  date d2(day_clock::universal_day());
  std::cout << "Getting UTC date: "
            <<  to_iso_string(d2) << std::endl;

  date::ymd_type ymd2 = day_clock::universal_day_ymd();
  std::cout << ymd2.year << "-" 
            << ymd2.month.as_long_string() << "-"
            << ymd2.day << std::endl;

  return 0;

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
