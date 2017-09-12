// Boost.Signals library
//
// Copyright (C) 2002 Doug Gregor (gregod@cs.rpi.edu)
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
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime> 

std::vector<int> valuesOutput;
bool ungrouped1 = false;
bool ungrouped2 = false;
bool ungrouped3 = false;

struct emit_int {
  emit_int(int v) : value(v) {}

  void operator()() const 
  {
    BOOST_TEST(!ungrouped1 && !ungrouped2 && !ungrouped3);
    valuesOutput.push_back(value);
    std::cout << value << ' ';
  }

private:
  int value;
};

struct write_ungrouped1 {
  void operator()() const
  {
    BOOST_TEST(!ungrouped1);
    ungrouped1 = true;
    std::cout << "(Ungrouped #1)" << ' ';
  }
};

struct write_ungrouped2 {
  void operator()() const
  {
    BOOST_TEST(!ungrouped2);
    ungrouped2 = true;
    std::cout << "(Ungrouped #2)" << ' ';
  }
};

struct write_ungrouped3 {
  void operator()() const
  {
    BOOST_TEST(!ungrouped3);
    ungrouped3 = true;
    std::cout << "(Ungrouped #3)" << ' ';
  }
};

int test_main(int, char* [])
{
  using namespace std;
  srand(time(0));

  std::vector<int> sortedValues;

  boost::signal0<void> sig;
  sig.connect(write_ungrouped1());
  for (int i = 0; i < 100; ++i) {
    using namespace std;
    int v = rand() % 100;
    sortedValues.push_back(v);
    sig.connect(v, emit_int(v));

    if (i == 50) {
      sig.connect(write_ungrouped2());
    }
  }
  sig.connect(write_ungrouped3());

  std::sort(sortedValues.begin(), sortedValues.end());

  sig();
  std::cout << std::endl;

  BOOST_TEST(valuesOutput == sortedValues);
  BOOST_TEST(ungrouped1);
  BOOST_TEST(ungrouped2);
  BOOST_TEST(ungrouped3);
  return 0;
}
