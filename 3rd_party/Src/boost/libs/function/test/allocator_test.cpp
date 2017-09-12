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
#include <cassert>
#include <functional>
#include <boost/function.hpp>

using namespace std;
using namespace boost;

static int alloc_count = 0;
static int dealloc_count = 0;

template<typename T>
struct counting_allocator : public std::allocator<T>
{
  template<typename U>
  struct rebind
  {
    typedef counting_allocator<U> other;
  };

  
  T* allocate(std::size_t n)
  {
    alloc_count++;
    return std::allocator<T>::allocate(n);
  }

  void deallocate(T* p, std::size_t n)
  {
    dealloc_count++;
    std::allocator<T>::deallocate(p, n);
  }
};

static int do_minus(int x, int y) { return x-y; }

struct DoNothing
{
  void operator()() const {}
};

static void do_nothing() {}

int
test_main(int, char*[])
{
  function2<int, int, int, counting_allocator<int> > f; 
  f = plus<int>();
  f.clear();
  BOOST_TEST(alloc_count == 1);
  BOOST_TEST(dealloc_count == 1);

  alloc_count = 0;
  dealloc_count = 0;
  f = &do_minus;
  f.clear();

  function0<void, counting_allocator<int> > fv;
  alloc_count = 0;
  dealloc_count = 0;
  fv = DoNothing();
  fv.clear();
  BOOST_TEST(alloc_count == 1);
  BOOST_TEST(dealloc_count == 1);
  
  alloc_count = 0;
  dealloc_count = 0;
  fv = &do_nothing;
  fv.clear();

  return 0;
}
