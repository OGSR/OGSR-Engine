//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_WINTHREADS
//  TITLE:         MS Windows threads
//  DESCRIPTION:   The platform supports MS Windows style threads.

#include <windows.h>


namespace boost_has_winthreads{

int test()
{
   HANDLE h = CreateMutex(0, 0, 0);
   if(h != NULL)
   {
      WaitForSingleObject(h, INFINITE);
      ReleaseMutex(h);
      CloseHandle(h);
   }
   return 0;
}

}




