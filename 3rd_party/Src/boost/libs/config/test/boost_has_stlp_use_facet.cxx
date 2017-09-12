//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_STLP_USE_FACET
//  TITLE:         STLport version of use_facet
//  DESCRIPTION:   The standard library lacks a conforming std::use_facet,
//                 but has a workaound class-version that does the job.
//                 This is primarily for the STLport std lib.

#include <locale>


namespace boost_has_stlp_use_facet{

int test()
{
   std::locale l;
   const std::ctype<char>& ct = *std::_Use_facet<std::ctype<char> >(l);
   return 0;
}

}




