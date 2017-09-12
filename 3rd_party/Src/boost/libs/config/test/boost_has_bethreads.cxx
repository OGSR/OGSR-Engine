//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_BETHREADS
//  TITLE:         BeOS Threads
//  DESCRIPTION:   The platform supports BeOS style threads.

#include <OS.h>


namespace boost_has_bethreads{

int test()
{
   sem_id mut = create_sem(1, "test");
   if(mut > 0)
   {
      acquire_sem(mut);
      release_sem(mut);
      delete_sem(mut);
   }
   return 0;
}

}




