//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_MS_INT64_NUMERIC_LIMITS
//  TITLE:         std::numeric_limits<__int64>
//  DESCRIPTION:   The C++ implementation does not provide the a specialisation
//                 for std::numeric_limits<__int64>.

#include <limits>

namespace boost_no_ms_int64_numeric_limits{

int test()
{
   if(0 == std::numeric_limits<__int64>::is_specialized) return -1;
   if(0 == std::numeric_limits<unsigned __int64>::is_specialized) return -1;
   return 0;
}

}




