//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_NANOSLEEP
//  TITLE:         nanosleep
//  DESCRIPTION:   The platform supports POSIX API nanosleep.

#include <time.h>


namespace boost_has_nanosleep{

void f()
{
    // this is never called, it just has to compile:
    timespec ts = {0, 0};
    timespec rm;
    int res = nanosleep(&ts, &rm);
    (void) &res;
}

int test()
{
   return 0;
}

}




