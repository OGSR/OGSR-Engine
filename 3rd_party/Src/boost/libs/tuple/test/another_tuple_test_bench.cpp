// Copyright (C) 1999, 2000 Jaakko Järvi (jaakko.jarvi@cs.utu.fi)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies. 
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice 
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty, 
// and with no claim as to its suitability for any purpose.

// For more information, see http://www.boost.org


//  another_test_bench.cpp  --------------------------------

// This file has various tests to see that things that shouldn't
// compile, don't compile.

// Defining any of E1 to E5 or E7 to E11 opens some illegal code that 
// should cause the compliation to fail.

#define BOOST_INCLUDE_MAIN  // for testing, include rather than link
#include <boost/test/test_tools.hpp>    // see "Header Implementation Option"

#include "boost/tuple/tuple.hpp"

#include <string>
#include <utility>

using namespace std;
using namespace boost;
using namespace boost::tuples;


template<class T> void dummy(const T&) {}

class A {}; class B {}; class C {};

// A non-copyable class
class no_copy {
  no_copy(const no_copy&) {}
public:
  no_copy() {};
};

no_copy y;

#ifdef E1
tuple<no_copy> v1;  // should faild
#endif


#ifdef E2
char cs[10];
tuple<char[10]> v3;  // should fail, arrays must be stored as references
#endif

// a class without a public default constructor
class no_def_constructor {
  no_def_constructor() {}
public:
  no_def_constructor(std::string) {} // can be constructed with a string
};

void foo1() {

#ifdef E3
  dummy(tuple<no_def_constructor, no_def_constructor, no_def_constructor>()); 
  // should fail

#endif
}

void foo2() {
// testing default values
#ifdef E4
  dummy(tuple<double&>()); // should fail, not defaults for references
  dummy(tuple<const double&>()); // likewise
#endif

#ifdef E5
  double dd = 5;
  dummy(tuple<double&>(dd+3.14)); // should fail, temporary to non-const reference
#endif
}



// make_tuple ------------------------------------------


   void foo3() {
#ifdef E7
    std::make_pair("Doesn't","Work"); // fails
#endif
    //    make_tuple("Does", "Work"); // this should work
}



// - testing element access

void foo4() 
{
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
  double d = 2.7; 
  A a;
  tuple<int, double&, const A&> t(1, d, a);
  const tuple<int, double&, const A> ct = t;
  (void)ct;
#ifdef E8
  get<0>(ct) = 5; // can't assign to const
#endif

#ifdef E9
  get<4>(t) = A(); // can't assign to const
#endif
#ifdef E10
  dummy(get<5>(ct)); // illegal index
#endif

#endif
}

// testing copy and assignment with implicit conversions between elements
// testing tie

  class AA {};
  class BB : public AA {};
  struct CC { CC() {} CC(const BB& b) {} };
  struct DD { operator CC() const { return CC(); }; };

  void foo5() {
    tuple<char, BB*, BB, DD> t;
    (void)t;
    tuple<char, char> aaa;
    tuple<int, int> bbb(aaa);
    (void)bbb;
    //    tuple<int, AA*, CC, CC> a = t;
    //    a = t;
  }


// testing tie
// testing assignment from std::pair
void foo7() {

   tuple<int, int, float> a;
#ifdef E11
   a = std::make_pair(1, 2); // should fail, tuple is of length 3, not 2
#endif

    dummy(a);
}



// --------------------------------
// ----------------------------
int test_main(int, char *[]) {

  foo1();
  foo2();
  foo3();
  foo4();
  foo5();

  foo7();

  return 0;
}
