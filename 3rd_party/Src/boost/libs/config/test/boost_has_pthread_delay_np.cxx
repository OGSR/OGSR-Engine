//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_PTHREAD_DELAY_NP
//  TITLE:         pthread_delay_np
//  DESCRIPTION:   The platform supports non-standard pthread_delay_np API.

#include <pthread.h>
#include <time.h>


namespace boost_has_pthread_delay_np{

void f()
{
    // this is never called, it just has to compile:
    timespec ts;
    int res = pthread_delay_np(&ts);
}

int test()
{
   return 0;
}

}




