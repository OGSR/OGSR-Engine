//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_USE_FACET
//  TITLE:         std::use_facet
//  DESCRIPTION:   The standard library lacks a conforming std::use_facet.

#include <locale>

namespace boost_no_std_use_facet{

int test()
{
   std::locale l;
   const std::ctype<char>& ct = std::use_facet<std::ctype<char> >(l);
   (void)ct;
   return 0;
}

}




