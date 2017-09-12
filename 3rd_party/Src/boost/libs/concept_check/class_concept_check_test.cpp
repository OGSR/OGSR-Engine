// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/concept_check.hpp>

/*

  This file verifies that the BOOST_CLASS_REQUIRE macro of the Boost
  Concept Checking Library does not cause errors when it is not suppose
  to.

*/

struct foo { bool operator()(int) { return true; } };
struct bar { bool operator()(int, char) { return true; } };

class class_requires_test
{
  BOOST_CLASS_REQUIRE(int, boost, EqualityComparableConcept);
  typedef int* int_ptr; typedef const int* const_int_ptr;
  BOOST_CLASS_REQUIRE2(int_ptr, const_int_ptr, boost, EqualOpConcept);
  BOOST_CLASS_REQUIRE3(foo, bool, int, boost, UnaryFunctionConcept);
  BOOST_CLASS_REQUIRE4(bar, bool, int, char, boost, BinaryFunctionConcept);
};

int
main()
{
  class_requires_test x;
  boost::ignore_unused_variable_warning(x);
  return 0;
}
