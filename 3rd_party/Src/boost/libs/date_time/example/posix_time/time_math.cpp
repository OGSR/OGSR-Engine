//Some simple examples of constructing and calculating with times
//Output:
//2002-Feb-01 00:00:00 - 2002-Feb-01 05:04:02.001000000 = -5:04:02.001000000

#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

int
main() 
{
  using namespace boost::posix_time;
  using namespace boost::gregorian;

  date d(2002,Feb,1); //an arbitrary date
  //construct a time by adding up some durations durations
  ptime t1(d, hours(5)+minutes(4)+seconds(2)+millisec(1));
  //construct a new time by subtracting some times
  ptime t2 = t1 - hours(5)- minutes(4)- seconds(2)- millisec(1);
  //construct a duration by taking the difference between times
  time_duration td = t2 - t1;
    
  std::cout << to_simple_string(t2) << " - " 
            << to_simple_string(t1) << " = "
            << to_simple_string(td) << std::endl;
  
  return 0;
}

