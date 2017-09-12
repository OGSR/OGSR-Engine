//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_CLOCK_GETTIME
//  TITLE:         clock_gettime
//  DESCRIPTION:   The platform supports POSIX standard API clock_gettime.

#include <time.h>


namespace boost_has_clock_gettime{

void f()
{
    // this is never called, it just has to compile:
    timespec tp;
    int res = clock_gettime(CLOCK_REALTIME, &tp);
    (void) &res;
}

int test()
{
   return 0;
}

}




