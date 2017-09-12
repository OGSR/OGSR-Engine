//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_SIGACTION
//  TITLE:         sigaction
//  DESCRIPTION:   The platform supports POSIX standard API sigaction.

#include <signal.h>


namespace boost_has_sigaction{

void f()
{
    // this is never called, it just has to compile:
    struct sigaction* sa1;
    struct sigaction* sa2;
    int res = sigaction(0, sa1, sa2);
    (void) &res;
}

int test()
{
   return 0;
}

}




