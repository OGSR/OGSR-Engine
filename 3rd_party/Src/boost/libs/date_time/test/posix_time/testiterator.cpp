/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include "boost/date_time/gregorian/formatters.hpp"


int
main() 
{
  using namespace boost::posix_time;
  using namespace boost::gregorian;

  date d(2000,Jan,20);
  ptime start(d);
  const ptime answer1[] = {ptime(d), ptime(d,seconds(1)),
                           ptime(d,seconds(2)), ptime(d,seconds(3))};
  int i=0;
  time_iterator titr(start,seconds(1)); 
  for (; titr < ptime(d,seconds(4)); ++titr) {
    std::cout << to_simple_string(*titr) << std::endl;
    check("iterator -- 1 sec", answer1[i] == *titr);
    i++;
  }
  check("iterator -- 1 sec", i == 4); // check the number of iterations


  //iterate by hours
  const ptime answer2[] = {ptime(d), ptime(d,hours(1)),
                           ptime(d,hours(2)), ptime(d,hours(3))};
  i=0;
  time_iterator titr2(start,hours(1)); 
  for (; titr2 < ptime(d,hours(4)); ++titr2) {
    std::cout << to_simple_string(*titr2) << std::endl;
    check("iterator -- 1 hour", answer2[i] == *titr2);
    i++;
  }
  check("iterator -- 1 hour", i == 4); // check the number of iterations


  //iterate by 15 mintues
  const ptime answer3[] = {ptime(d), ptime(d,minutes(15)),
                           ptime(d,minutes(30)), ptime(d,minutes(45)),
                           ptime(d,minutes(60)), ptime(d,minutes(75))};
  i=0;
  time_iterator titr3(start,minutes(15)); 
  for (; titr3 < ptime(d,time_duration(1,20,0)); ++titr3) {
    std::cout << to_simple_string(*titr3) << std::endl;
    check("iterator -- 15 min", answer3[i] == *titr3);
    i++;
  }
  check("iterator -- 15 min", i == 6); // check the number of iterations

  //iterate by .1 seconds
  const ptime answer4[] = {ptime(d), ptime(d,time_duration(0,0,0,1000)),
                           ptime(d,time_duration(0,0,0,2000)),
                           ptime(d,time_duration(0,0,0,3000))};
  i=0;
  time_iterator titr4(start,time_duration(0,0,0,1000)); 
  for (; titr4 < ptime(d,time_duration(0,0,0,4000)); ++titr4) {
    std::cout << to_simple_string(*titr4) << std::endl;
    check("iterator -- tenth sec", answer4[i] == *titr4);
    i++;
  }
  check("iterator -- tenth sec", i == 4); // check the number of iterations


 
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
