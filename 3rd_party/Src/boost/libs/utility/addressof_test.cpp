// Copyright (C) 2002 Brad King (brad.king@kitware.com) 
//                    Doug Gregor (gregod@cs.rpi.edu)
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

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>
#include <boost/utility.hpp>

struct useless_type {};

class nonaddressable {
public:
  void dummy(); // Silence GCC warning: all member of class are private

private:
  useless_type operator&() const;
};

int test_main(int, char*[])
{
  nonaddressable* px = new nonaddressable();

  nonaddressable& x = *px;
  BOOST_TEST(boost::addressof(x) == px);

  const nonaddressable& cx = *px;
  BOOST_TEST(boost::addressof(cx) == static_cast<const nonaddressable*>(px));

  volatile nonaddressable& vx = *px;
  BOOST_TEST(boost::addressof(vx) == static_cast<volatile nonaddressable*>(px));

  const volatile nonaddressable& cvx = *px;
  BOOST_TEST(boost::addressof(cvx) == static_cast<const volatile nonaddressable*>(px));
  
  return 0;
}
