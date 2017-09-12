/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include <iostream>

int
main() 
{

  using namespace boost::gregorian;

  partial_date pd1(1,Jan);
  date d = pd1.get_date(2000);
  check("Partial date getdate",     date(2000,1,1) == d);
  d = pd1.get_date(2001);
  check("Partial date getdate",     date(2001,1,1) == d);
  partial_date pd2(1,Feb);
  check("Partial date operator==",  pd1 == pd1);
  check("Partial date operator==",  !(pd1 == pd2));
  check("Partial date operator==",  !(pd2 == pd1));
  check("Partial date operator<",   !(pd1 < pd1));
  check("Partial date operator<",    pd1 < pd2);
  check("Partial date operator<",   !(pd2 < pd1));

  typedef boost::date_time::last_kday_of_month<date> lastkday;

  //Find last Sunday in Feb
  lastkday lsif(Sunday, Feb);
  std::cout << to_simple_string(lsif.get_date(2002)) << std::endl; //24th
  check("Last kday",     date(2002,Feb,24) == lsif.get_date(2002));
  lastkday ltif(Thursday, Feb);
  check("Last kday",     date(2002,Feb,28) == ltif.get_date(2002));
  lastkday lfif(Friday, Feb);
  check("Last kday",     date(2002,Feb,22) == lfif.get_date(2002));

  typedef boost::date_time::first_kday_of_month<date> firstkday;

  firstkday fsif(Sunday, Feb);
  std::cout << to_simple_string(fsif.get_date(2002)) << std::endl; //24th
  check("First kday",     date(2002,Feb,3) == fsif.get_date(2002));
  firstkday ftif(Thursday, Feb);
  check("First kday",     date(2002,Feb,7) == ftif.get_date(2002));
  firstkday ffif(Friday, Feb);
  check("First kday",     date(2002,Feb,1) == ffif.get_date(2002));
  
  typedef boost::date_time::first_kday_after<date> firstkdayafter;
  firstkdayafter fkaf(Monday);
  std::cout << to_simple_string(fkaf.get_date(date(2002,Feb,1)))
            << std::endl; //feb 4
  check("kday after",date(2002,Feb,4) == fkaf.get_date(date(2002,Feb,1)));
  firstkdayafter fkaf2(Thursday);
  check("kday after",date(2002,Feb,7) == fkaf2.get_date(date(2002,Feb,1)));
  check("kday after",date(2002,Feb,28)== fkaf2.get_date(date(2002,Feb,21)));

  typedef boost::date_time::first_kday_before<date> firstkdaybefore;
  firstkdaybefore fkbf(Monday);
  std::cout << to_simple_string(fkaf.get_date(date(2002,Feb,10)))
            << std::endl; //feb 4
  check("kday before",date(2002,Feb,4) == fkbf.get_date(date(2002,Feb,10)));
  firstkdaybefore fkbf2(Thursday);
  check("kday before",date(2002,Jan,31) == fkbf2.get_date(date(2002,Feb,1)));
  check("kday before",date(2002,Feb,7)== fkbf2.get_date(date(2002,Feb,14)));
  
  return printTestStats();

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
