/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

int
main() 
{

  using namespace boost::posix_time;
  ptime tl = second_clock::local_time();
  std::cout << to_simple_string(tl) << std::endl;

  ptime tu = second_clock::universal_time();
  std::cout << to_simple_string(tu) << std::endl;

#if (defined(_POSIX_TIMERS))
  for (int i=0; i < 3; ++i) {
    ptime t2 = second_clock::local_time();
    std::cout << to_simple_string(t2) << std::endl;
    sleep(1);
  }
#endif

//   for (int i=0; i < 10; ++i) {
//     ptime t2 = high_precision_clock::local_time();
//     std::cout << to_simple_string(t2) << std::endl;
//   }


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
