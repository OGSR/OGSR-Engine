//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_PTHREAD_YIELD
//  TITLE:         pthread_yield
//  DESCRIPTION:   The platform supports non standard API pthread_yield.

#include <pthread.h>


namespace boost_has_pthread_yield{

void f()
{
    // this is never called, it just has to compile:
    int res = pthread_yield();
}

int test()
{
   return 0;
}

}




