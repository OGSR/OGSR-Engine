//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_LOCALE
//  TITLE:         std::locale
//  DESCRIPTION:   The standard library lacks std::locale.

#include <locale>

namespace boost_no_std_locale{

int test()
{
   std::locale l1;
   //
   // ideally we would construct a locale from a facet,
   // but that requires template member functions which 
   // may not be available, instead just check that we can
   // construct a pointer to a facet:
   //
   const std::ctype<char>* pct = 0;
   (void) &l1;
   (void) &pct;
   return 0;
}

}




