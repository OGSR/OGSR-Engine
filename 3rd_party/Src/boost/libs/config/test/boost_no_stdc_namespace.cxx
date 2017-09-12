//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STDC_NAMESPACE
//  TITLE:         std:: namespace for C API's
//  DESCRIPTION:   The contents of C++ standard headers for C library
//                 functions (the <c...> headers) have not been placed
//                 in namespace std.  This test is difficult - some libraries
//                 "fake" the std C functions by adding using declarations
//                 to import them into namespace std, unfortunately they don't
//                 necessarily catch all of them...

#include <cstring>
#include <cctype>
#include <ctime>

#undef isspace
#undef isalpha
#undef ispunct

namespace boost_no_stdc_namespace{


int test()
{
   char c = 0;
#ifndef BOOST_NO_CTYPE_FUNCTIONS
   std::isspace(c);
   std::isalpha(c);
   std::ispunct(c);
#endif
   std::strlen(&c);
   std::clock();

   return 0;
}

}
