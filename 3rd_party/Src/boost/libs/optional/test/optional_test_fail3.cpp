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
#include "boost/optional.hpp"

//
// THIS TEST SHOULD FAIL TO COMPILE
//
#if BOOST_WORKAROUND( BOOST_INTEL_CXX_VERSION, <= 700) // Intel C++ 7.0
// Interl C++ 7.0 incorrectly accepts the initialization "boost::optional<int> opt = 3"
// even though the ctor is explicit (c.f. 12.3.1.2), so the test uses another form of
// copy-initialization: argument-passing (8.5.12)
void helper ( boost::optional<int> ) ;
void test_explicit_constructor()
{
  helper(3) ; // ERROR: Ctor is explicit.
}
#else
void test_explicit_constructor()
{
  boost::optional<int> opt = 3 ; // ERROR: Ctor is explicit.
}
#endif


