//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_SWPRINTF
//  TITLE:         swprintf
//  DESCRIPTION:   The platform does not have a conforming version of swprintf.

#include <wchar.h>
#include <stdio.h>


namespace boost_no_swprintf{

int test()
{
   wchar_t buf[10];
   swprintf(buf, 10, L"%d", 10);
   return 0;
}

}





