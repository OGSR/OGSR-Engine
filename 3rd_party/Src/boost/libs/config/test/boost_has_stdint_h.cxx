//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_STDINT_H
//  TITLE:         stdint.h
//  DESCRIPTION:   There are no 1998 C++ Standard headers <stdint.h> 
//                 or <cstdint>, although the 1999 C Standard does
//                 include <stdint.h>.
//                 If <stdint.h> is present, <boost/stdint.h> can make
//                 good use of it, so a flag is supplied (signalling
//                 presence; thus the default is not present, conforming
//                 to the current C++ standard).

#include <stdint.h>

namespace boost_has_stdint_h{

int test()
{
   int8_t i = 0;
   int_fast8_t j = 0;
   int_least8_t k = 0;
   (void)i;
   (void)j;
   (void)k;

   return 0;
}

}




