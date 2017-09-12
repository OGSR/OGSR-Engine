//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STRINGSTREAM
//  TITLE:         <sstream>
//  DESCRIPTION:   The C++ implementation does not provide the <sstream> header.

#include <sstream>
#include <string>

namespace boost_no_stringstream{

int test()
{
   std::stringstream ss;
   ss << "abc";
   std::string s = ss.str();
   return (s != "abc");
}

}



