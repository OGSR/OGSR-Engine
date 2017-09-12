// Simple program that uses the gregorian calendar to find the last
// day of the month.

#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>


int
main()
{
  
  std::cout << "   Enter Year(ex: 2002): ";
  int year, month;
  std::cin >> year;
  std::cout << "   Enter Month(1..12): ";
  std::cin >> month;
  try {
    int day = boost::gregorian::gregorian_calendar::end_of_month_day(year,month);
    boost::gregorian::date endOfMonth(year,month,day);
    std::cout << boost::gregorian::to_simple_string(endOfMonth) << std::endl;

    //Iterate thru by months --
    boost::gregorian::month_iterator mitr(endOfMonth,1);
    boost::gregorian::date startOfNextYear(year+1,1,1);
    //loop thru the days and print each one
    while (mitr < startOfNextYear){
      std::cout << boost::gregorian::to_simple_string(*mitr) << std::endl;
      ++mitr;
    }

  }
  catch(...) {
    std::cout << "Invalid Date Entered" << std::endl;
  }
  return 0;

}
