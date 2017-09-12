//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
//  TITLE:         pthread_mutexattr_settype
//  DESCRIPTION:   The platform supports POSIX API pthread_mutexattr_settype.

#include <pthread.h>


namespace boost_has_pthread_mutexattr_settype{

void f()
{
    // this is never called, it just has to compile:
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    int type = 0;
    pthread_mutexattr_settype(&attr, type);
}

int test()
{
   return 0;
}

}





