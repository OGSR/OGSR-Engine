//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_LONG_LONG
//  TITLE:         long long
//  DESCRIPTION:   The platform supports long long.

#include <cstdlib>


namespace boost_has_long_long{

int test()
{
   long long lli = 0LL;
   unsigned long long ulli = 0uLL;
   (void)lli;
   (void)ulli;
   return 0;
}

}




