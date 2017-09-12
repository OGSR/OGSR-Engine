/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include <iostream>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/testfrmwk.hpp"


int
main() 
{
  using namespace boost::posix_time;
  using namespace boost::gregorian;

  date d(2001,Dec,1);
  time_duration td(5,4,3);
  ptime t1(d, td);     //2001-Dec-1 05:04:03
  check("date part check", t1.date() == d);
  check("time part check", t1.time_of_day() == td);
  ptime t2(t1); //copy constructor
  ptime t3 = t2; //assignment
  check("date part check", t3.date() == d);
  check("time part check", t3.time_of_day() == td);
  check("equality", t1 == t3);
  date d2(2001,Jan,1);
  ptime t4(d2, td);  //2001-Jan-1 05:04:03
  check("equality - not equal", !(t1 == t4));
  time_duration td1(5,4,0);
  ptime t5(d, td1); //2001-Dec-1 05:04:00
  check("equality - not equal", !(t1 == t5));
  check("not equal - not equal", t1 != t5);

  check("less - not less",       !(t1 < t1));
  check("less - less",           t4 < t1);
  check("less - less",           t5 < t1);
  check("less equal - equal",    t1 <= t1);
  check("greater equal - equal", t1 >= t1);

  date_duration twodays(2);
  ptime t6 = t1 + twodays;
  date d3(2001,Dec,3);
  check("operator+(date_duration)", t6 == ptime(d3,td));
  ptime t7 = t1 - twodays;
  check("operator-(date_duration)", t7 == ptime(date(2001,Nov,29),td));
  time_duration td2(1,2,3); 
  ptime t8(date(2001,Dec,1)); //midnight
  ptime t9 = t8 + td2; //Dec 2 at 01:02:03
  ptime t10(date(2001,Dec,1),time_duration(1,2,3));
  std::cout << to_simple_string(t9) << std::endl;
  std::cout << to_simple_string(t10) << std::endl;
  std::cout << to_simple_string(td2) << std::endl;
  check("add 2001-Dec-01 0:0:0 + 01:02:03", t9 == t10);
  time_duration td3(24,0,0); // a day
  check("add 2001-Dec-01 0:0:0 + 24:00:00", t8+td3 == ptime(date(2001,Dec,2)));
  time_duration td4(24,0,1); // a day, 1 second
  check("add 2001-Dec-01 0:0:0 + 24:00:01", t8+td4
        == ptime(date(2001,Dec,2), time_duration(0,0,1)));
  //looks like this fails b/c limits are exceeded now that we have subseconds..
  time_duration td5(168,0,1);  //one week 24X7
   check("add 2001-Dec-01 0:0:0 + 168:00:01", t8+td5
         == ptime(date(2001,Dec,8), time_duration(0,0,1)));

//   ptime t10a = t8+td5;
//   std::cout << to_simple_string(t10a) << std::endl;

  //Subtraction of time duration -- add more!!
  ptime t11(date(2001,Dec,1), time_duration(12,0,0)); //noon
  time_duration td6(12,0,1);
  ptime t12 = t11-td6;
  check("sub 2001-Dec-01 12:0:0 - 12:00:01", 
        t12 == ptime(date(2001,Nov,30), time_duration(23,59,59)));

  check("sub 2001-Dec-01 12:0:0 - 13:00:00", 
        (t11-time_duration(13,0,0))== ptime(date(2001,Nov,30), 
                                            time_duration(23,0,0)));
  //  std::cout << to_simple_string(t12.date()) << std::endl;

  ptime t13(d, hours(3));
  ptime t14(d, hours(4));
  ptime t14a(d+date_duration(1), hours(4));
  //Subtract 2 times
  std::cout << to_simple_string(t14-t13) << std::endl;
  //  time_duration td7 = 
  check("time subtraction positive result", 
        t14-t13 == hours(1));
  std::cout << to_simple_string(t13-t14) << std::endl;
  check("time subtraction negative result", 
        t13-t14 == hours(-1));
  check("time subtraction positive result", 
        t14a-t14 == hours(24));
  

  ptime t15(d, time_duration(0,0,0,1));
  ptime t16(d, time_duration(0,0,0,2));
  check("time subsecond add test", 
        t15 + time_duration::unit() == t16);
  check("time subsecond sub test", 
        t16 - time_duration::unit() == t15);
 
  ptime t17 = ptime(d) - time_duration::unit();
  std::cout << to_simple_string(t17) << std::endl;

  ptime t18(d, hours(25));
  std::cout << to_simple_string(t18) << std::endl;

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
