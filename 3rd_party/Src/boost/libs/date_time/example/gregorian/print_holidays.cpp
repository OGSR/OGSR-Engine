/*generate a set of dates using a collections of date generators
Output looks like:
Enter Year: 2002
2002-Jan-01 [Tue]
2002-Jan-21 [Mon]
2002-Feb-12 [Tue]
2002-Jul-04 [Thu]
2002-Sep-02 [Mon]
2002-Nov-28 [Thu]
2002-Dec-25 [Wed]
Number Holidays: 7
*/


#include "boost/date_time/gregorian/gregorian.hpp"
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include <set>


void
print_date(boost::gregorian::date d) 
{
  std::cout << d << " [" << d.day_of_week() << "]\n";
}


int
main() {

  std::cout << "Enter Year: ";
  int year;
  std::cin >> year;


  using namespace boost::gregorian;

  //define a collection of holidays fixed by month and day
  std::vector<partial_date> holidays;
  holidays.push_back(partial_date(1,Jan)); //Western New Year
  holidays.push_back(partial_date(4,Jul)); //US Independence Day
  holidays.push_back(partial_date(25, Dec));//Christmas day


  //define a shorthand for the nkday function object
  typedef boost::date_time::nth_kday_of_month<date> nkday;
  //define a collection of nth kday holidays
  std::vector<nkday> more_holidays;
  more_holidays.push_back(nkday(nkday::first,  Monday,   Sep)); //US labor day
  more_holidays.push_back(nkday(nkday::third,  Monday,   Jan)); //MLK Day
  more_holidays.push_back(nkday(nkday::second, Tuesday,  Feb)); //Pres day
  more_holidays.push_back(nkday(nkday::fourth, Thursday, Nov)); //Thanksgiving

  typedef std::set<date> date_set;
  date_set all_holidays;
  
#if (defined(BOOST_MSVC) && (_MSC_VER <= 1200))  // 1200 == VC++ 6.0
  std::cout << "Sorry, this example temporarily disabled on VC 6.\n"
            << "The std::transform isn't accepted by the compiler\n"
            << "So if you hack up the example without std::transform\n"
            << "it should work\n"
            << std::endl; 
  
#else

  //generate a set of concrete dates from the 'partial_date' holidays
  //by calling the get_date functions
  std::transform(holidays.begin(), holidays.end(),
                 std::insert_iterator<date_set>(all_holidays, all_holidays.begin()),
                 std::bind2nd(std::mem_fun_ref(&partial_date::get_date),
                              year));

  //Now add in the 'floating' holidays 
  std::transform(more_holidays.begin(), more_holidays.end(),
                 std::insert_iterator<date_set>(all_holidays, all_holidays.begin()),
                 std::bind2nd(std::mem_fun_ref(&nkday::get_date),
                              year));



  //print the holidays to the screen
  std::for_each(all_holidays.begin(), all_holidays.end(), print_date);
  std::cout << "Number Holidays: " << all_holidays.size() << std::endl;

#endif

return 0;

}
