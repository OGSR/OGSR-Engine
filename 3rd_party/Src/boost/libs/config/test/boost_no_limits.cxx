//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_LIMITS
//  TITLE:         <limits>
//  DESCRIPTION:   The C++ implementation does not provide the <limits> header.

#include <limits>

namespace boost_no_limits{

int test()
{
   if(0 == std::numeric_limits<int>::is_specialized) return -1;
   if(0 == std::numeric_limits<long>::is_specialized) return -1;
   if(0 == std::numeric_limits<char>::is_specialized) return -1;
   if(0 == std::numeric_limits<unsigned char>::is_specialized) return -1;
   if(0 == std::numeric_limits<signed char>::is_specialized) return -1;
   if(0 == std::numeric_limits<float>::is_specialized) return -1;
   if(0 == std::numeric_limits<double>::is_specialized) return -1;
   if(0 == std::numeric_limits<long double>::is_specialized) return -1;
   return 0;
}

}




