/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include <iostream>

void test_base_iterator(boost::gregorian::date end,
                        boost::gregorian::date_iterator& di,
                        std::string& data)
{
  using namespace boost::gregorian;
  for (; di < end; ++di) {
    data += to_iso_string(*di) + " ";
  }
};
 

int
main() 
{
  using namespace boost::gregorian;

  day_iterator di(date(2002,Jan,1));
  std::string data;
  test_base_iterator(date(2002,Jan,3),di,data);
  month_iterator di2(date(2002,Jan,3));
  test_base_iterator(date(2002,Mar,1),di2,data);
  std::string result("20020101 20020102 20020103 20020203 ");
  check("base iterator", data == result); 
  std::cout << data << std::endl;


  typedef boost::date_time::day_functor<date> dfg;
  
  {
    const date DayAnswers[] = {date(2000,Jan,20),date(2000,Jan,22),date(2000,Jan,24)};
    boost::date_time::date_itr<dfg,date> ditr(date(2000,Jan,20),2);
    int i=0;
    for (; ditr < date(2000,Jan,25); ++ditr) {
      //std::cout << *ditr << " ";
      check("day iterator -- 2 days", DayAnswers[i] == *ditr);
      i++;
    }
    check("day iterator -- 2 days", i == 3); // check the number of iterations
  }

  typedef boost::date_time::week_functor<date> wfg;
  {
    const date WeekAnswers[] = {date(2000,Jan,20),date(2000,Jan,27),date(2000,Feb,3)};
    boost::date_time::date_itr<wfg, date> ditr(date(2000,Jan,20));
    int i=0;
    for (; ditr < date(2000,Feb,6); ++ditr) {
      //std::cout << *ditr << " ";
      check("week iterator", WeekAnswers[i] == *ditr);
      i++;
    }
    check("week iterator", i == 3);
  }

  {
    const date WeekAnswers[] = {date(2000,Jan,20),date(2000,Feb,3)};
    boost::date_time::date_itr<wfg, date> ditr(date(2000,Jan,20),2);
    int i=0;
    for (; ditr < date(2000,Feb,6); ++ditr) {
      //std::cout << *ditr << " ";
      check("week iterator", WeekAnswers[i] == *ditr);
      i++;
    }
    check("week iterator", i == 2);
  }

  {
    const date WeekAnswers[] = {date(2000,Jan,20),date(2000,Feb,3), date(2000,Feb,17)};
    boost::date_time::date_itr<wfg, date> ditr(date(2000,Jan,20),2);
    int i=0;
    for (; ditr < date(2000,Feb,20); ++ditr) {
      //std::cout << *ditr << " ";
      check("week iterator -- 2 weeks", WeekAnswers[i] == *ditr);
      i++;
    }
    check("week iterator -- 2 weeks", i == 3);
  }
  
  typedef boost::date_time::month_functor<date> mfg;
  {
    const date MonthAnswers[] = {
      date(2000,Jan,1),date(2000,Feb,1),date(2000,Mar,1),date(2000,Apr,1),
      date(2000,May,1),date(2000,Jun,1),date(2000,Jul,1),date(2000,Aug,1),
      date(2000,Sep,1),date(2000,Oct,1),date(2000,Nov,1),date(2000,Dec,1),
      date(2001,Jan,1)
    };
    
    boost::date_time::date_itr<mfg, date> ditr(date(2000,Jan,1));
    int i = 0;
    try { 
      for (; ditr < date(2001,Jan,2); ++ditr) {
        check("month iterator: " + to_iso_string(*ditr), MonthAnswers[i] == *ditr);
        i++;
      }
      check("month iterator iteration count", i == 13);
    }
    catch(std::exception& e) 
    {
      check("month iterator: exception failure", false);      
      std::cout << e.what() << std::endl;
    }
  }

  {
    const date MonthAnswers[] = {
      date(2000,Jan,31),date(2000,Feb,29),date(2000,Mar,31),date(2000,Apr,30),
      date(2000,May,31),date(2000,Jun,30),date(2000,Jul,31),date(2000,Aug,31),
      date(2000,Sep,30),date(2000,Oct,31),date(2000,Nov,30),date(2000,Dec,31),
      date(2001,Jan,31)
    };
    
    boost::date_time::date_itr<mfg, date> ditr(date(2000,Jan,31));
    int i = 0;
    try { 
      for (; ditr < date(2001,Feb,1); ++ditr) {
        //      std::cout << *ditr << " ";
        check("last day of month iterator: " + to_iso_string(*ditr), 
              MonthAnswers[i] == *ditr);
        //check("last day of month iterator", MonthAnswers[i] == *ditr);
        i++;
      }
      check("last day of month iterator", i == 13);
    }
    catch(std::exception& e) 
    {
      check("last day of month iterator: exception failure", false);      
      std::cout << e.what() << std::endl;
    }
  }

  {
    const date MonthAnswers[] = {
      date(2000,Feb,29),date(2000,Mar,31),date(2000,Apr,30),
      date(2000,May,31),date(2000,Jun,30),date(2000,Jul,31),date(2000,Aug,31),
      date(2000,Sep,30),date(2000,Oct,31),date(2000,Nov,30),date(2000,Dec,31),
      date(2001,Jan,31)
    };
    
    boost::date_time::date_itr<mfg, date> ditr(date(2000,Feb,29));
    int i = 0;
    try { 
      for (; ditr < date(2001,Feb,1); ++ditr) {
        //      std::cout << *ditr << " ";
        check("last day of month iterator2: " + to_iso_string(*ditr), 
              MonthAnswers[i] == *ditr);
        //check("last day of month iterator", MonthAnswers[i] == *ditr);
        i++;
      }
      check("last day of month iterator2", i == 12);
    }
    catch(std::exception& e) 
    {
      check("last day of month iterator: exception failure", false);      
      std::cout << e.what() << std::endl;
    }
  }

  {
    const date MonthAnswers[] = {
      date(2000,Feb,28),date(2000,Mar,28),date(2000,Apr,28),
      date(2000,May,28),date(2000,Jun,28),date(2000,Jul,28),date(2000,Aug,28),
      date(2000,Sep,28),date(2000,Oct,28),date(2000,Nov,28),date(2000,Dec,28),
      date(2001,Jan,28)
    };
    
    boost::date_time::date_itr<mfg, date> ditr(date(2000,Feb,28));
    int i = 0;
    try { 
      for (; ditr < date(2001,Feb,1); ++ditr) {
        //      std::cout << *ditr << " ";
        check("last day of month iterator3: " + to_iso_string(*ditr), 
              MonthAnswers[i] == *ditr);
        //check("last day of month iterator", MonthAnswers[i] == *ditr);
        i++;
      }
      check("last day of month iterator3", i == 12);
    }
    catch(std::exception& e) 
    {
      check("last day of month iterator: exception failure", false);      
      std::cout << e.what() << std::endl;
    }
  }

  typedef boost::date_time::year_functor<date> yfg;
  {
    const date YearAnswers[] = {
      date(2000,Jan,1),date(2001,Jan,1),date(2002,Jan,1),date(2003,Jan,1),
      date(2004,Jan,1),date(2005,Jan,1),date(2006,Jan,1),date(2007,Jan,1),
      date(2008,Jan,1),date(2009,Jan,1),date(2010,Jan,1)
    };
    
    boost::date_time::date_itr<yfg, date> d3(date(2000,Jan,1));
    int i = 0;
    for (; d3 < date(2010,Jan,2); ++d3) {
      //std::cout << *d3 << " ";
      check("year iterator", YearAnswers[i] == *d3);
      i++;
    }
 }

  {
    const date YearAnswers[] = {
      date(2000,Jan,1),date(2002,Jan,1),
      date(2004,Jan,1),date(2006,Jan,1),
      date(2008,Jan,1),date(2010,Jan,1)
    };
    
    boost::date_time::date_itr<yfg, date> d3(date(2000,Jan,1),2);
    int i = 0;
    for (; d3 < date(2010,Jan,2); ++d3) {
      //std::cout << *d3 << " ";
      check("year iterator", YearAnswers[i] == *d3);
      i++;
    }
}

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
