//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_TWO_ARG_USE_FACET
//  TITLE:         two argument version of use_facet
//  DESCRIPTION:   The standard library lacks a conforming std::use_facet,
//                 but has a two argument version that does the job.
//                 This is primarily for the Rogue Wave std lib.

#include <locale>


namespace boost_has_two_arg_use_facet{

int test()
{
   std::locale l;
   const std::ctype<char>& ct = std::use_facet(l, (std::ctype<char>*)0);
   return 0;
}

}





