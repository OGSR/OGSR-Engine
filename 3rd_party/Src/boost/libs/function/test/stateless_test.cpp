// Boost.Function library

// Copyright (C) 2001 Doug Gregor (gregod@cs.rpi.edu)
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
#include <boost/function.hpp>
#include <stdexcept>

struct stateless_integer_add {
  int operator()(int x, int y) const { return x+y; }

  void* operator new(std::size_t, stateless_integer_add*)
  {
    throw std::runtime_error("Cannot allocate a stateless_integer_add");
    return 0; // suppress warnings
  }

  void operator delete(void*, stateless_integer_add*) throw()
  {
  }
};

namespace boost {
  template<>
  struct is_stateless<stateless_integer_add> {
    BOOST_STATIC_CONSTANT(bool, value = true);
  };
}

int test_main(int, char*[])
{
  boost::function2<int, int, int> f;
  f = stateless_integer_add();

  return 0;
}
