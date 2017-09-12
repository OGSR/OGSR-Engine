

/* Copyright (c) 2000 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/greg_weekday.hpp"

namespace boost {
namespace gregorian {

  const char* const short_weekday_names[]={"Sun", "Mon", "Tue", 
                                           "Wed", "Thu", "Fri", "Sat"};
  const char* const long_weekday_names[]= {"Sunday","Monday","Tuesday",
                                           "Wednesday", "Thursday",
                                           "Friday", "Saturday"};


  //! Return a 3 digit english string of the day of week (eg: Sun)
  const char*
  greg_weekday::as_short_string() const 
  {
    return short_weekday_names[value_];
  }
  //! Return a point to a long english string representing day of week
  const char*
  greg_weekday::as_long_string()  const 
  {
    return long_weekday_names[value_];
  }
  
} } //namespace gregorian

/* Copyright (c) 2000
 * CrystalClear Software, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  CrystalClear Software makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */
