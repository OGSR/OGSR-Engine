

// Simple program that uses the gregorian calendar to find the last
// day of the month.

#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>


int
main()
{

  using namespace boost::gregorian;
  typedef boost::date_time::month_functor<date> add_month;

  date d = day_clock::local_day();
  add_month mf(1);
  date d2 = d + mf.get_offset(d);
  std::cout << to_simple_string(d2) << std::endl;

  return 0;

}
