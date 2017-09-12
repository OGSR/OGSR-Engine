// (C) 2003, Fernando Luis Cacciola Carballal.
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied. Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#include<string>

#include "boost/optional.hpp"

//
// THIS TEST SHOULD FAIL TO COMPILE
//
void test_no_unsupported_conversion()
{
  boost::optional<int> opt1(1) ;
  boost::optional< std::string > opt2( opt1 ) ; // Cannot convert from "int" to "std::string"
}


