//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_MESSAGES
//  TITLE:         std::messages<charT>
//  DESCRIPTION:   The standard library lacks a conforming std::messages facet.

#include <locale>


namespace boost_no_std_messages{

//
// this just has to complile, not run:
//
void test_messages(const std::messages<char>& m)
{
   static const std::locale l;
   static const std::string name("foobar");
   m.close(m.open(name, l));
}

int test()
{
   const std::messages<char>* pmf = 0;
   (void) &pmf;
   return 0;
}

}




