//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_MACRO_USE_FACET
//  TITLE:         macro version of use_facet: _USE
//  DESCRIPTION:   The standard library lacks a conforming std::use_facet,
//                 but has a macro _USE(loc, Type) that does the job.
//                 This is primarily for the Dinkumware std lib.

#include <locale>

#ifndef _USE
#error "macro _USE not defined"
#endif

namespace boost_has_macro_use_facet{

int test()
{
   std::locale l;
   const std::ctype<char>& ct = std::_USE(l, std::ctype<char>);
   return 0;
}

}





