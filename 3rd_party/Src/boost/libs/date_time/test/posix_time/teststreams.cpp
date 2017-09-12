/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/testfrmwk.hpp"

const char* const de_short_month_names[]={"Jan","Feb","Mar","Apr","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez", "NAM"};

const char* const de_long_month_names[]={"Januar","Februar","Marz","April","Mai","Juni","Juli","August","September","Oktober","November","Dezember","NichtDerMonat"};
const char* const de_special_value_names[]={"NichtDatumzeit","-unbegrenztheit", "+unbegrenztheit"};

const char* const de_short_weekday_names[]={"Son", "Mon", "Die","Mit", "Don", "Fre", "Sam"};

const char* const de_long_weekday_names[]={"Sonntag", "Montag", "Dienstag","Mittwoch", "Donnerstag", "Freitag", "Samstag"};



int
main() 
{
#ifndef BOOST_DATE_TIME_NO_LOCALE
  using namespace boost::gregorian;
  using namespace boost::posix_time;
  std::stringstream ss;
  date d1(2002,May,1);
  ptime t1(d1, hours(12)+minutes(10)+seconds(5));
  ss << t1;
  check("check time output: "+ss.str(), 
        ss.str() == std::string("2002-May-01 12:10:05"));

  //Send out the same time with german
  std::locale global;
  typedef boost::date_time::all_date_names_put<greg_facet_config> date_facet;
  std::locale german(global, 
                     new date_facet(de_short_month_names, 
                                    de_long_month_names,
                                    de_special_value_names,
                                    de_short_weekday_names,
                                    de_long_weekday_names,
                                    '.',
                                    boost::date_time::ymd_order_dmy,
                                    boost::date_time::month_as_short_string));
 
  ss.imbue(german); 
  ss.str("");
  ss << t1;
  check("check time output: "+ss.str(), 
        ss.str() == std::string("01.Mai.2002 12:10:05"));


  time_duration td(5,4,3);
  ss.str("");
  ss << td;
  check("check time period output: "+ss.str(), 
        ss.str() == std::string("05:04:03"));

  ss.imbue(global); 
  time_period tp(t1, ptime(d1, hours(23)+time_duration::unit()));
  ss.str("");
  ss << tp;
  check("check time period output: "+ss.str(), 
        ss.str() == std::string("[2002-May-01 12:10:05/2002-May-01 23:00:00]"));


#else
  check("All pass, no tests executed - Locales not supported", true);

#endif //BOOST_DATE_TIME_NO_LOCALE

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
