//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_PTHREADS
//  TITLE:         POSIX Threads
//  DESCRIPTION:   The platform supports POSIX style threads.

#include <pthread.h>


namespace boost_has_pthreads{

int test()
{
   pthread_mutex_t mut;
   int result = pthread_mutex_init(&mut, 0);
   if(0 == result)
   {
      pthread_mutex_lock(&mut);
      pthread_mutex_unlock(&mut);
      pthread_mutex_destroy(&mut);
   }
   return 0;
}

}




