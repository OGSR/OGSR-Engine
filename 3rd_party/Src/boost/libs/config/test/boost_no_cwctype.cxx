//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_CWCTYPE
//  TITLE:         <wctype.h> and <cwctype>
//  DESCRIPTION:   The Platform does not provide <wctype.h> and <cwctype>.

//
// Note that some platforms put these prototypes in the wrong headers,
// we have to include pretty well all the string headers on the chance that
// one of them will contain what we want!
//
#include <cwctype>
#include <wctype.h>
#include <cwchar>
#include <wchar.h>
#include <cctype>
#include <ctype.h>

namespace boost_no_cwctype{

int test()
{
   if(!(iswalpha(L'a') &&
        iswcntrl(L'\r') &&
        iswdigit(L'2') &&
        iswlower(L'a') &&
        iswpunct(L',') &&
        iswspace(L' ') &&
        iswupper(L'A') &&
        iswxdigit(L'A')
   )) return -1;
   if(L'a' != towlower(L'A')) return -1;
   return 0;
}

}





