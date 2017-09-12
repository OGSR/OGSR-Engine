//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_DISTANCE
//  TITLE:         std::distance
//  DESCRIPTION:   The platform does not have a conforming version of std::distance.

#include <algorithm>
#include <iterator>


namespace boost_no_std_distance{

int test()
{
   const char* begin = 0;
   const char* end = 0;
   if(std::distance(begin, end)) return -1;
   return 0;
}

}




