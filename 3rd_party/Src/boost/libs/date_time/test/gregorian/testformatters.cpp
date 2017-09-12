/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/testfrmwk.hpp"

int
main() 
{

  boost::gregorian::date d1(2002,01,02);
  std::string ds1 = boost::gregorian::to_simple_string(d1);
  check("check string:     " + ds1, ds1 == "2002-Jan-02");
 
  std::string ids1(boost::gregorian::to_iso_string(d1));
  //  std::cout << boost::gregorian::to_iso_string(d1) << std::endl;
  check("check iso normal: " + ids1, ids1 == "20020102");

  std::string sds1 = boost::gregorian::to_sql_string(d1);
  check("check sql string: "+sds1, sds1 == "2002-01-02");

  boost::gregorian::date d2(2001,12,30);
  std::string ds2 = boost::gregorian::to_simple_string(d2);
  check("check string:     "+ds2, ds2 == "2001-Dec-30");
  std::string ids2 = boost::gregorian::to_iso_extended_string(d2);
  check("check iso extended string: "+ids2, ids2 == "2001-12-30");

  //TODO REMOVE THIS STUFF
  //  std::stringstream os;
  // typedef boost::gregorian::date::ymd_type ymd_type;
  
//   typedef boost::date_time::ymd_formatter<ymd_type, boost::date_time::simple_format> ymdf;
//   ymd_type ymd = d1.year_month_day();
//   ymdf::ymd_put(ymd, os);
//   check("ostream low level check string", 
//     os.str() == std::string("2002-Jan-02"));


//   typedef boost::date_time::date_formatter<boost::gregorian::date, 
//                                       boost::date_time::simple_format> datef;
//   os.str("");
//   datef::date_to_ostream(d1, os);
//   check("ostream low level check string", 
//     os.str() == std::string("2002-Jan-02"));
  
   
  using namespace boost::gregorian;
  date d3(neg_infin);
  check("check negative infinity",     
        (to_simple_string(d3) == std::string("-infinity  ")));
  date d4(pos_infin);
  check("check positive infinity",     
        (to_simple_string(d4) == std::string("+infinity  ")));
  date d5(not_a_date_time);
  std::cout << to_simple_string(d5) << "|" << std::endl;
  check("check not a date",     
        (to_simple_string(d5) == std::string("not-a-date")));

  date_period p1(date(2000,Jan,1), date(2001,Jan,1));
  check("check period format",     
        (to_simple_string(p1) == std::string("[2000-Jan-01/2000-Dec-31]")));
  date_period p2(date(2000,Jan,1), date(pos_infin));
  check("check period format",     
        (to_simple_string(p2) == std::string("[2000-Jan-01/+infinity  ]")));
  std::cout << to_simple_string(p2) << std::endl;


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
