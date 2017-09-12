//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_MS_INT64
//  TITLE:         __int64
//  DESCRIPTION:   The platform supports Microsoft style __int64.

#include <cstdlib>


namespace boost_has_ms_int64{

int test()
{
   __int64 lli = 0i64;
   unsigned __int64 ulli = 0ui64;
   (void)lli;
   (void)ulli;
   return 0;
}

}





