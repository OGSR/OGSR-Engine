
#include <iostream>
#include "boost/date_time/gregorian/gregorian.hpp"

int
main() 
{
  
  using namespace boost::gregorian;

  date today = day_clock::local_day();
  //Subtract two dates to get a duration
  date_duration days_since_year_start = today - date(today.year(),Jan,1);
  std::cout << "Days since Jan 1: " << days_since_year_start.days() 
            << std::endl;
  return 0;
};
