//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_CTYPE_FUNCTIONS
//  TITLE:         functions in <ctype.h>
//  DESCRIPTION:   The Platform does not provide functions for the character-
//                 classifying operations in <ctype.h>. Some platforms provide
//                 macros and don't provide functions. Under C++ it's an error
//                 to provide the macros at all, but that's a separate issue.

#include <ctype.h>

namespace boost_no_ctype_functions {

extern "C" {
  typedef int (* character_classify_function)(int);
}

void pass_function(character_classify_function)
{
}

int test()
{
   pass_function(isalpha);
   pass_function(isalnum);
   pass_function(iscntrl);
   pass_function(isdigit);
   pass_function(isgraph);
   pass_function(islower);
   pass_function(isprint);
   pass_function(ispunct);
   pass_function(isspace);
   pass_function(isupper);
   pass_function(isxdigit);
   return 0;
}

}
