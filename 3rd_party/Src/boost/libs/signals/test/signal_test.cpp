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
#include <functional>
#include <iostream>

template<typename T>
struct max_or_default {
  typedef T result_type;
  template<typename InputIterator>
  typename InputIterator::value_type
  operator()(InputIterator first, InputIterator last) const
  {
    if (first == last)
      return T();

    T max = *first++;
    for (; first != last; ++first)
      max = (*first > max)? *first : max;

    return max;
  }
};

struct make_int {
  make_int(int n, int cn) : N(n), CN(cn) {}

  int operator()() { return N; }
  int operator()() const { return CN; }

  int N;
  int CN;
};

template<int N>
struct make_increasing_int {
  make_increasing_int() : n(N) {}

  int operator()() const { return n++; }

  mutable int n;
};

static void
test_zero_args()
{
  make_int i42(42, 41);
  make_int i2(2, 1);
  make_int i72(72, 71);
  make_int i63(63, 63);
  make_int i62(62, 61);

  {
    boost::signal<int (), max_or_default<int> > s0;

    std::cout << "sizeof(signal) = " << sizeof(s0) << std::endl;
    boost::signals::connection c2 = s0.connect(i2);
    boost::signals::connection c72 = s0.connect(72, i72);
    boost::signals::connection c62 = s0.connect(60, i62);
    boost::signals::connection c42 = s0.connect(i42);

    BOOST_TEST(s0() == 72);

    s0.disconnect(72);
    BOOST_TEST(s0() == 62);

    c72.disconnect(); // Double-disconnect should be safe
    BOOST_TEST(s0() == 62);

    s0.disconnect(72); // Triple-disconect should be safe
    BOOST_TEST(s0() == 62);

    // Also connect 63 in the same group as 62
    s0.connect(60, i63);
    BOOST_TEST(s0() == 63);

    // Disconnect all of the 60's
    s0.disconnect(60);
    BOOST_TEST(s0() == 42);

    c42.disconnect();
    BOOST_TEST(s0() == 2);

    c2.disconnect();
    BOOST_TEST(s0() == 0);
  }  

  {
    boost::signal<int (), max_or_default<int> > s0;
    boost::signals::connection c2 = s0.connect(i2);
    boost::signals::connection c72 = s0.connect(i72);
    boost::signals::connection c62 = s0.connect(i62);
    boost::signals::connection c42 = s0.connect(i42);

    const boost::signal<int (), max_or_default<int> >& cs0 = s0;
    BOOST_TEST(cs0() == 72);
  }  

  {
    make_increasing_int<7> i7;
    make_increasing_int<10> i10;

    boost::signal<int (), max_or_default<int> > s0;
    boost::signals::connection c7 = s0.connect(i7);
    boost::signals::connection c10 = s0.connect(i10);

    BOOST_TEST(s0() == 10);
    BOOST_TEST(s0() == 11);
  }  
}

static void
test_one_arg()
{
  boost::signal<int (int value), max_or_default<int> > s1;

  s1.connect(std::negate<int>());
  s1.connect(std::bind1st(std::multiplies<int>(), 2));

  BOOST_TEST(s1(1) == 2);
  BOOST_TEST(s1(-1) == 1);
}

static void
test_signal_signal_connect()
{
  boost::signal<int (int value), max_or_default<int> > s1;

  s1.connect(std::negate<int>()); 

  BOOST_TEST(s1(3) == -3);

  {
    boost::signal<int (int value), max_or_default<int> > s2;
    s1.connect(s2);
    s2.connect(std::bind1st(std::multiplies<int>(), 2));
    s2.connect(std::bind1st(std::multiplies<int>(), -3));

    BOOST_TEST(s2(-3) == 9);
    BOOST_TEST(s1(3) == 6);
  } // s2 goes out of scope and disconnects

  BOOST_TEST(s1(3) == -3);
}

int
test_main(int, char* [])
{
  test_zero_args();
  test_one_arg();
  test_signal_signal_connect();
  return 0;
}
