//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_CWCHAR
//  TITLE:         <wchar.h> and <cwchar>
//  DESCRIPTION:   The Platform does not provide <wchar.h> and <cwchar>.

#include <cwchar>
#include <wchar.h>

namespace boost_no_cwchar{

int test()
{
   wchar_t c1[2] = { 0 };
   wchar_t c2[2] = { 0 };
   if(wcscmp(c1,c2) || wcslen(c1)) return -1;
   wcscpy(c1,c2);
   wcsxfrm(c1,c2,0);
   return 0;
}

}





