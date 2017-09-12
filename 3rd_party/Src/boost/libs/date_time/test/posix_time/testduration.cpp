/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "boost/date_time/posix_time/time_formatters.hpp"
#include "boost/date_time/testfrmwk.hpp"


int
main() 
{
  using namespace boost::posix_time;

  
//   std::cout << "Default limits: SECOND " << std::endl;
  {
    time_duration td;
    check("default construction -- 0 ticks", td.ticks() == 0);
//     check("default construction -- 0 secs", td.seconds() == 0);
//     check("default construction -- 0 min", td.minutes() == 0); 
  }
  time_duration t_1(0,1,40);
  time_duration t_2(0,1,41);
  check("less test", !(t_2 < t_2));
  check("equal test", t_1 == t_1);
  check("greater equal - equal", t_1 >= t_1);
  check("greater equal - greater", t_2 >= t_1);
  check("less equal - equal  ", t_2 <= t_2);
  check("greater             ", t_2 > t_1);
  check("greater - not       ", !(t_1 > t_2));
  time_duration t_3(t_2);
  check("copy constructor    ", t_2 == t_3);
  time_duration t_4 = t_3;
  check("assignment operator ", t_2 == t_4);
  
  time_duration t_5 = t_2 + t_1;
  //VC6 goes ambiguous on the next line...
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0
  //sorry ticks() doesn't work on VC6
#else
  std::cout << t_5.ticks() << std::endl;  
#endif
  check("add", t_5 == time_duration(0,3,21));
  
  time_duration t_6(5,4,3); //05:04:03
  check("h-m-s 5-4-3 hours", t_6.hours() == 5);
  check("h-m-s 5-4-3 hours", t_6.minutes() == 4);
  check("h-m-s 5-4-3 hours", t_6.seconds() == 3);

  hours tenhours(10);
  minutes fivemin(5);
  time_duration t7 = time_duration(1,2,3) + tenhours + fivemin;
  check("short hand durations add", t7 == time_duration(11,7,3));
  time_duration t8 = tenhours + time_duration(1,2,3) +  fivemin;
  check("short hand durations add", t8 == time_duration(11,7,3));

  if (time_duration::resolution() >= boost::date_time::micro) {
    time_duration t_9(5,4,3,9876); //05:04:03.09876
    check("h-m-s 5-4-3.21 hours",  t_9.hours() == 5);
    check("h-m-s 5-4-3.21 min  ",  t_9.minutes() == 4);
    check("h-m-s 5-4-3.21 sec  ",  t_9.seconds() == 3);
    check("h-m-s 5-4-3.21 fs   ",  t_9.fractional_seconds() == 9876);
    //  check("h-m-s 5-4-3.21 fs   ",  t_9.fs_as_double() == 0.9876);
    //std::cout << t_9.fs_as_double() << std::endl;
    std::cout << to_simple_string(t_9) << std::endl;
  }

  if (time_duration::resolution() >= boost::date_time::tenth) {
    time_duration t_10(5,4,3,9); //05:04:03.00001
    check("h-m-s 5-4-3.9 hours",  t_10.hours() == 5);
    check("h-m-s 5-4-3.9 min  ",  t_10.minutes() == 4);
    check("h-m-s 5-4-3.9 sec  ",  t_10.seconds() == 3);
    check("h-m-s 5-4-3.9 fs   ",  t_10.fractional_seconds() == 9);
    std::cout << to_simple_string(t_10) << std::endl;
  }

  if (time_duration::resolution() >= boost::date_time::milli) {
    millisec ms(9);
    //  time_duration t_10(0,0,0,); //00:00:00.009
    std::cout << "time_resolution: " << time_duration::resolution() << std::endl;
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0
  //sorry res_adjust() doesn't work on VC6
#else
    std::cout << "res_adjust " << time_res_traits::res_adjust() << std::endl;
#endif
    if (time_duration::resolution() == boost::date_time::nano) {
      check("millisec",  ms.fractional_seconds() == 9000000);
    }
    else {
      check("millisec 9000",  ms.fractional_seconds() == 9000);
    }
  }

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
  if (time_duration::resolution() >= boost::date_time::nano) {
    nanosec ns(9);
    //  time_duration t_10(0,0,0,); //00:00:00.00009
    check("nanosec",  ns.fractional_seconds() == 9);
    std::cout << to_simple_string(ns) << std::endl;
    time_duration ns18 = ns + ns;
    check("nanosec",  ns18.fractional_seconds() == 18);
    std::cout << to_simple_string(ns18) << std::endl;
    nanosec ns2(1000000000); //one second
    check("nano to second compare",  ns2 == seconds(1));
    std::cout << to_simple_string(ns2) << std::endl;
  }
#endif  

  time_duration t_11(3600,0,0); 
  check("3601 hours   ",  t_11.hours() == 3600);

  using namespace boost::gregorian;
  ptime t1(date(2001,7,14));
  ptime t2(date(2002,7,14));
  check("One year of hours: 365*24=8760",  365*24 == ((t2-t1).hours()));

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
