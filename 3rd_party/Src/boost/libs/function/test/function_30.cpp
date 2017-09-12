// Boost.Function library

// Copyright (C) 2002-2003 Doug Gregor (gregod@cs.rpi.edu)
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

// Make sure we don't try to redefine function2
#include <boost/function/function2.hpp>

// Define all Boost.Function class templates up to 30 arguments
#define BOOST_FUNCTION_MAX_ARGS 30
#include <boost/function.hpp>

int main()
{
  boost::function0<float> f0;

  boost::function30<float, int, int, int, int, int, int, int, int, int, int,
                    int, int, int, int, int, int, int, int, int, int,
                    int, int, int, int, int, int, int, int, int, int> f30;
  return 0;
}
