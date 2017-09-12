//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_FTIME
//  TITLE:         GetSystemTimeAsFileTime
//  DESCRIPTION:   The platform supports Win32 API GetSystemTimeAsFileTime.

#include <windows.h>


namespace boost_has_ftime{

void f()
{
    // this is never called, it just has to compile:
   FILETIME ft;
   GetSystemTimeAsFileTime(&ft);
}

int test()
{
   return 0;
}

}




