//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_MIN_MAX
//  TITLE:         std::min and std::max
//  DESCRIPTION:   The C++ standard library does not provide
//                 the min() and max() template functions that
//                 should be in <algorithm>.

#include <algorithm>

namespace boost_no_std_min_max{


int test()
{
   int i = 0;
   int j = 2;
   if(std::min(i,j) != 0) return -1;
   if(std::max(i,j) != 2) return -1;

   return 0;
}

}



