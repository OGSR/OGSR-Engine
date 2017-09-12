/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/greg_year.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include <iostream>


int
main() 
{

  using namespace boost::gregorian;
  greg_year d1(1400);
  check("Basic of min", d1 == 1400);
  greg_year d2(10000);
  check("Basic test of max", d2 == 10000);
  try {
    greg_year bad(0);
    check("Bad year creation", false); //oh oh, fail
    //unreachable
    std::cout << "Shouldn't reach here: " << bad << std::endl;
  }
  catch(std::exception &) {
    check("Bad year creation", true); //good
    
  }
  try {
    greg_year bad(10001);
    check("Bad year creation2", false); //oh oh, fail
    //unreachable
    std::cout << "Shouldn't reach here: " << bad << std::endl;
  }
  catch(std::exception&) {
    check("Bad year creation2", true); //good
    
  }
  check("traits min year", greg_year::min() ==  1400);
  check("traits max year", greg_year::max() == 10000);

  printTestStats();
  return 0;
};

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
