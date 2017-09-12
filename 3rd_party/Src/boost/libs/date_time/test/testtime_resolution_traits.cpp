/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/time_resolution_traits.hpp"
#include "boost/date_time/testfrmwk.hpp"


int
main() 
{
  using namespace boost::date_time;
  check("milli traits num digits",  milli_res::num_fractional_digits() == 3);
  check("milli traits resolution adjust",  
        milli_res::res_adjust() == 1000);
  check("milli tick calculations",  
        milli_res::to_tick_count(0,0,0,1) == 1);
  check("milli tick calculations",  
        milli_res::to_tick_count(0,0,1,1) == 1001);
  check("milli tick calculations",  
        milli_res::to_tick_count(0,1,0,0) == 60000);
  boost::int64_t one_hour_milli = 3600*1000;
  check("milli tick calculations",  
        milli_res::to_tick_count(1,0,0,0) == one_hour_milli);
  
  check("micro traits num digits",  micro_res::num_fractional_digits() == 6);
  check("micro traits resolution adjust",  
        micro_res::res_adjust() == 1000000);
  check("micro tick calculations",  
        micro_res::to_tick_count(0,0,0,1) == 1);
  check("micro tick calculations",  
        micro_res::to_tick_count(0,0,1,1) == 1000001);
  check("micro tick calculations",  
        micro_res::to_tick_count(0,1,0,0) == 60000000);
  boost::int64_t one_hour_micro = 3600*1000;
  one_hour_micro = one_hour_micro*1000; //avoid compiler overflow!
  check("micro tick calculations",  
        micro_res::to_tick_count(1,0,0,0) == one_hour_micro);

  check("nano traits num digits",  nano_res::num_fractional_digits() == 9);
  check("nano traits resolution adjust",  
        nano_res::res_adjust() == 1000000000);
  check("nano tick calculations",  
        nano_res::to_tick_count(0,0,0,1) == 1);
  check("nano tick calculations",  
        nano_res::to_tick_count(0,0,1,1) == 1000000001);
#if ((defined(__GNUC__) && (__GNUC__ < 3)) || defined(__IBMCPP__))
  check("nano tick calculations",  
        nano_res::to_tick_count(0,1,0,0) == 60000000000LL);
#else
  check("nano tick calculations",  
        nano_res::to_tick_count(0,1,0,0) == 60000000000);
#endif

  //skip io on VC6 b/c of lack of operator<< for int64
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0
#else
  std::cout << one_hour_micro << std::endl;
#endif
  boost::int64_t one_hour_nano = one_hour_micro*1000;
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0
#else
  std::cout << one_hour_nano << std::endl;
#endif
  check("nano tick calculations",  
        nano_res::to_tick_count(1,0,0,0) == one_hour_nano);


  printTestStats();
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
