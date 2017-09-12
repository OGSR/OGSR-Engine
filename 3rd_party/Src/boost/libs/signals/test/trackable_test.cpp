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

struct short_lived : public boost::signals::trackable {
  ~short_lived() {}
};

struct swallow {
  template<typename T> int operator()(const T*, int i) { return i; }
};

template<typename T>
struct max_or_default {
  typedef T result_type;

  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    if (first == last)
      return T();

    T max = *first++;
    for (; first != last; ++first)
      max = (*first > max)? *first : max;

    return max;
  }
};

int test_main(int, char*[])
{
  typedef boost::signal1<int, int, max_or_default<int> > sig_type;
  sig_type s1;
 
  // Test auto-disconnection 
  BOOST_TEST(s1(5) == 0);
  {
    short_lived shorty;
    s1.connect(boost::bind<int>(swallow(), &shorty, _1));
    BOOST_TEST(s1(5) == 5);
  }
  BOOST_TEST(s1(5) == 0);
  
  // Test auto-disconnection of slot before signal connection
  {
    short_lived* shorty = new short_lived();

    sig_type::slot_type slot(boost::bind<int>(swallow(), shorty, _1));
    delete shorty;

    BOOST_TEST(s1(5) == 0);
  }

  return 0;
}
