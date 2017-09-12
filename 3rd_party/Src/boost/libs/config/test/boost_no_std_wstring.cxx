//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_WSTRING
//  TITLE:         std::wstring
//  DESCRIPTION:   The standard library lacks std::wstring.

#include <string>

namespace boost_no_std_wstring{

int test()
{
   std::wstring s;
   if(*s.c_str() || (s.begin() != s.end()) || s.size() || (sizeof(std::wstring::value_type) != sizeof(wchar_t))) return -1;
   return 0;
}

}




