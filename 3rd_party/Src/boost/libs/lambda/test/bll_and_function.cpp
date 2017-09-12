//  bll_and_function.cpp  - The Boost Lambda Library -----------------------
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

// test using BLL and boost::function

#define BOOST_INCLUDE_MAIN  // for testing, include rather than link
#include <boost/test/test_tools.hpp>    // see "Header Implementation Option"

#include "boost/lambda/lambda.hpp"

#include "boost/function.hpp"

#include <vector>
#include <map>
#include <set>
#include <string>


using namespace boost::lambda;

using namespace std;

void test_function() {

  boost::function<int (int, int)> f;
  f = _1 + _2;

 BOOST_TEST(f(1, 2)== 3);

 int i=1; int j=2;
 boost::function<int& (int&, int)> g = _1 += _2;
 g(i, j);
 BOOST_TEST(i==3);



  int* sum = new int();
  *sum = 0;
  boost::function<int& (int)> counter = *sum += _1;
  counter(5); // ok, sum* = 5;
  BOOST_TEST(*sum == 5);
  delete sum; 
  
  // The next statement would lead to a dangling reference
  // counter(3); // error, *sum does not exist anymore

}


int test_main(int, char *[]) {

  test_function();

  return 0;
}






