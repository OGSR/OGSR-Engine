//  cast_tests.cpp  -- The Boost Lambda Library ------------------
//
// Copyright (C) 2000-2003 Jaakko Järvi (jaakko.jarvi@cs.utu.fi)
// Copyright (C) 2000-2003 Gary Powell (powellg@amazon.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies. 
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice 
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty, 
// and with no claim as to its suitability for any purpose.
//
// For more information, see www.boost.org

// -----------------------------------------------------------------------


#define BOOST_INCLUDE_MAIN  // for testing, include rather than link
#include <boost/test/test_tools.hpp>    // see "Header Implementation Option"


#include "boost/lambda/lambda.hpp"

#include "boost/lambda/casts.hpp"

#include <string>

using namespace boost::lambda;
using namespace std;

class base {
  int x;
public:
  virtual std::string class_name() const { return "const base"; }
  virtual std::string class_name() { return "base"; }

};

class derived : public base {
  int y[100];
public:
  virtual std::string class_name() const { return "const derived"; }
  virtual std::string class_name() { return "derived"; }
};




void do_test() {

  derived *p_derived = new derived;
  base *p_base = new base;

  base *b = 0;
  derived *d = 0;

  (var(b) = ll_static_cast<base *>(p_derived))();
  (var(d) = ll_static_cast<derived *>(b))();
  
  BOOST_TEST(b->class_name() == "derived");
  BOOST_TEST(d->class_name() == "derived");

  (var(b) = ll_dynamic_cast<derived *>(b))();
  BOOST_TEST(b != 0);
  BOOST_TEST(b->class_name() == "derived");

  (var(d) = ll_dynamic_cast<derived *>(p_base))();
  BOOST_TEST(d == 0);

  

  const derived* p_const_derived = p_derived;

  BOOST_TEST(p_const_derived->class_name() == "const derived");
  (var(d) = ll_const_cast<derived *>(p_const_derived))();
  BOOST_TEST(d->class_name() == "derived");

  int i = 10;
  char* cp = reinterpret_cast<char*>(&i);

  int* ip;
  (var(ip) = ll_reinterpret_cast<int *>(cp))();
  BOOST_TEST(*ip == 10);


  // typeid 

  BOOST_TEST(string(ll_typeid(d)().name()) == string(typeid(d).name()));

 
  // sizeof

  BOOST_TEST(ll_sizeof(_1)(p_derived) == sizeof(p_derived));
  BOOST_TEST(ll_sizeof(_1)(*p_derived) == sizeof(*p_derived));
  BOOST_TEST(ll_sizeof(_1)(p_base) == sizeof(p_base));
  BOOST_TEST(ll_sizeof(_1)(*p_base) == sizeof(*p_base));

  int an_array[100];
  BOOST_TEST(ll_sizeof(_1)(an_array) == 100 * sizeof(int));

  delete p_derived;
  delete p_base;


}

int test_main(int, char *[]) {

  do_test();
  return 0;
}
