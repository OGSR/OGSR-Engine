//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_GETTIMEOFDAY
//  TITLE:         gettimeofday
//  DESCRIPTION:   The platform supports POSIX standard API gettimeofday.

#include <sys/time.h>


namespace boost_has_gettimeofday{

void f()
{
    // this is never called, it just has to compile:
    timeval tp;
    int res = gettimeofday(&tp, 0);
    (void) &res;
}

int test()
{
   return 0;
}

}




