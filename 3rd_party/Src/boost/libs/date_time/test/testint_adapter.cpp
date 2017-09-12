/* Copyright (c) 2001 CrystalClear Software, Inc.
 * Disclaimer & Full Copyright at end of file
 * Author: Jeff Garland 
 */

#include "boost/date_time/int_adapter.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include "boost/cstdint.hpp"
#include <iostream>

template<typename int_type> 
void print()
{
  //MSVC 6 has problems with this, but it's not really important 
  //so we will just skip them....
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0

#else
  std::cout << "min:       " << int_type::min().as_number() << std::endl;
  std::cout << "max:       " << int_type::max().as_number() << std::endl;
  std::cout << "neg_infin: " << 
    int_type::neg_infinity().as_number() << std::endl;
  std::cout << "pos_infin: " << 
    int_type::pos_infinity().as_number() << std::endl;
  std::cout << "not a number:  " << 
    int_type::not_a_number().as_number() << std::endl;
#endif
}


template<typename int_type> 
void test_int()
{
  int_type i = int_type::neg_infinity();

  check("is infinity", i.is_infinity());
  check("as_special convert", boost::date_time::neg_infin == i.as_special() );
  check("as_special convert", boost::date_time::neg_infin == int_type::to_special(i.as_number()) );
  i = int_type::pos_infinity();
  check("is infinity", i.is_infinity());
  check("as_special convert", boost::date_time::pos_infin == i.as_special() );
  i = 1;
  check("is infinity", !i.is_infinity());
  int_type j = int_type::neg_infinity();
  check("infinity less",     j < i);
  check("infinity less",     !(j < j));
  check("infinity greater",  (i > j));
  check("infinity equal",    !(j == i));
  check("infinity equal",    j == j);

  int_type k = 1;
  check("as_special convert", boost::date_time::not_special == k.as_special() );
  check("equal",             i == k);
  check("infinity not equal",    i != int_type::neg_infinity());
  check("infinity not equal",    i != int_type::pos_infinity());
  int_type l = i + int_type::pos_infinity();
  check("add infinity" ,         l == int_type::pos_infinity());
  check("add 2",                 (i + 2) == 3);
  i = int_type::not_a_number();
  check("as_special convert", boost::date_time::not_a_date_time == i.as_special() );
  check("add not a number",      (i + 2) == int_type::not_a_number());
  check("sub not a number",      (i - 2) == int_type::not_a_number());
  check("sub from infin",        (l - 2) == int_type::pos_infinity());
  i = 5;
  check("add zero ",             (i + 0) == 5);
  check("sub from 5-2 ",         (5 - 2) == 3);
  //  std::cout << i.as_number() << std::endl;
  check("from special ", 
        int_type::from_special(boost::date_time::pos_infin) == int_type::pos_infinity());
  check("from special ", 
        int_type::from_special(boost::date_time::neg_infin) == int_type::neg_infinity());
  check("from special ", 
        int_type::from_special(boost::date_time::not_a_date_time) == int_type::not_a_number());
  check("from special ", 
        int_type::from_special(boost::date_time::min_date_time) == int_type::min());
  check("from special ", 
        int_type::from_special(boost::date_time::max_date_time) == int_type::max());
}

int
main() 
{
  using namespace boost::date_time;

  print< int_adapter<unsigned long> >();
  test_int< int_adapter<unsigned long> >();
  print< int_adapter<long> >();
  test_int< int_adapter<long> >();
  print< int_adapter<boost::int64_t> >();
  test_int< int_adapter<boost::int64_t> >();


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
