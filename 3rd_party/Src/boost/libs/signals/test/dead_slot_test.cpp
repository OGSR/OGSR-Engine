// Boost.Signals library
//
// Copyright (C) 2001-2002 Doug Gregor (gregod@cs.rpi.edu)
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
#include <boost/signal.hpp>
#include <boost/bind.hpp>

typedef boost::signal1<int, int> sig_type;

class with_constant : public boost::signals::trackable {
public:
  with_constant(int c) : constant(c) {}

  int add(int i) { return i + constant; }

private:
  int constant;
};

void do_delayed_connect(with_constant* wc,
                        sig_type& sig,
                        sig_type::slot_type slot)
{
  // Should invalidate the slot, so that we cannot connect to it
  delete wc;

  boost::signals::connection c = sig.connect(slot);
  BOOST_TEST(!c.connected());
}

int test_main(int, char*[])
{
  sig_type s1;
  with_constant* wc1 = new with_constant(7);

  do_delayed_connect(wc1, s1, boost::bind(&with_constant::add, wc1, _1));

  return 0;
}
