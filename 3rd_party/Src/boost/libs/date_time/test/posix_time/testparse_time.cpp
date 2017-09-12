/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/testfrmwk.hpp"


int 
main()
{
  using namespace boost::gregorian;
  using namespace boost::posix_time;

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS

  std::string s1("12:11:10.123456789");
  time_duration td1= duration_from_string(s1);
  check("parse time duration: " + s1, 
        td1 == time_duration(12,11,10,123456789));
  std::string s2("12:11:10,123456789");
  time_duration td2= boost::date_time::parse_delimited_time_duration<time_duration>(s2);
  check("parse time duration: " + s2, 
        td2 == time_duration(12,11,10,123456789));
  std::string s3("12:11:10");
  time_duration td3= boost::date_time::parse_delimited_time_duration<time_duration>(s3);
  check("parse time duration: " + s3, 
        td3 == time_duration(12,11,10,0));
  std::string s4("23:59:59.000000001");
  time_duration td4= boost::date_time::parse_delimited_time_duration<time_duration>(s4);
  check("parse time duration: " + s4, 
        td4 == time_duration(23,59,59)+nanosec(1));
  std::string s5("23:59:59.999999999");
  time_duration td5= boost::date_time::parse_delimited_time_duration<time_duration>(s5);
  check("parse time duration: " + s5, 
        td5 == time_duration(23,59,59)+nanosec(999999999));
#endif

#if defined(BOOST_DATE_TIME_HAS_MICROSECONDS) && (!defined(BOOST_DATE_TIME_HAS_NANOSECONDS))
  {
    std::string s1("12:11:10.123456");
    time_duration td1= duration_from_string(s1);
    check("parse time duration: " + s1, 
          td1 == time_duration(12,11,10,123456));
    std::string s2("12:11:10,123456");
    time_duration td2= boost::date_time::parse_delimited_time_duration<time_duration>(s2);
    check("parse time duration: " + s2, 
          td2 == time_duration(12,11,10,123456));
    std::string s3("12:11:10");
    time_duration td3= boost::date_time::parse_delimited_time_duration<time_duration>(s3);
    check("parse time duration: " + s3, 
          td3 == time_duration(12,11,10,0));
    std::string s4("23:59:59.000001");
    time_duration td4= boost::date_time::parse_delimited_time_duration<time_duration>(s4);
    check("parse time duration: " + s4, 
          td4 == time_duration(23,59,59)+microsec(1));
    std::string s5("23:59:59.999999");
    time_duration td5= boost::date_time::parse_delimited_time_duration<time_duration>(s5);
    check("parse time duration: " + s5, 
          td5 == time_duration(23,59,59)+microsec(999999));
  }
#endif

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS

  std::string ts2("2002-12-31 00:00:00.999999999");
  ptime t2 = time_from_string(ts2);
  check("parse time: " + ts2, 
        t2 == ptime(date(2002,12,31),time_duration(0,0,0)+nanosec(999999999)));
#endif


  std::string date_1, tod_1;
  std::string ts1("2002-01-20 23:59:59.000");
  boost::date_time::split(ts1, ' ', date_1, tod_1);
  check("split function date part of " + ts1, 
        date_1 == std::string("2002-01-20"));
  check("time part of " + ts1, 
        tod_1 == std::string("23:59:59.000"));
//   std::cout << date_1 << "|" << std::endl;
//   std::cout << tod_1  << "|" << std::endl;


  ptime t1 = time_from_string(ts1);
  check("parse time: " + ts1, 
        t1 == ptime(date(2002,1,20),time_duration(23,59,59)));

  std::string s6("235859");
  time_duration td6= boost::date_time::parse_undelimited_time_duration<time_duration>(s6);
  check("parse time duration: " + s6, 
        td6 == time_duration(23,58,59));

  std::string ts3("20020120T235859");
  ptime t20 = from_iso_string(ts3);
  check("parse iso time: " + ts3, 
        t20 == ptime(date(2002,1,20),time_duration(23,58,59)));
  

  std::string ts4("19001231T000000");
  ptime t21 = from_iso_string(ts4);
  check("parse iso time: " + ts4, 
        t21 == ptime(date(1900,12,31),time_duration(0,0,0)));


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
