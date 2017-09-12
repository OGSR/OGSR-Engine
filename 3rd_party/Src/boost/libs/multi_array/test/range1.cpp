//
// range1.cpp - test of index_range
//


#include "boost/multi_array/index_range.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"
#include <cstddef>

int
test_main(int,char*[])
{
  typedef boost::detail::multi_array::index_range<int,std::size_t> range;

  {
    // typical range creation and extraction
    range r1(-3,5);
    BOOST_TEST(r1.start() == -3);
    BOOST_TEST(r1.finish() == 5);
    BOOST_TEST(r1.stride() == 1);
    BOOST_TEST(!r1.is_degenerate());
    BOOST_TEST(r1.get_start(0) == -3);
    BOOST_TEST(r1.get_finish(100) == 5);
  }

  {
    range r2(-3,5,2);
    BOOST_TEST(r2.start() == -3);
    BOOST_TEST(r2.finish() == 5);
    BOOST_TEST(r2.stride() == 2);
    BOOST_TEST(!r2.is_degenerate());
  }

  {
    // degenerate creation
    range r3(5);
    BOOST_TEST(r3.start() == 5);
    BOOST_TEST(r3.finish() == 5);
    BOOST_TEST(r3.stride() == 1);
    BOOST_TEST(r3.is_degenerate());
  }

  {
    // default range creation
    range r4;
    BOOST_TEST(r4.get_start(0) == 0);
    BOOST_TEST(r4.get_finish(100) == 100);
    BOOST_TEST(r4.stride() == 1);
  }

  {
    // create a range using the setter methods
    range r5 = range().stride(2).start(-3).finish(7);
    BOOST_TEST(r5.start() == -3);
    BOOST_TEST(r5.stride() == 2);
    BOOST_TEST(r5.finish() == 7);
  }

  // try out all the comparison operators
  {
    range r6 = -3 <= range().stride(2) < 7;
    BOOST_TEST(r6.start() == -3);
    BOOST_TEST(r6.stride() == 2);
    BOOST_TEST(r6.finish() == 7);
  }

  {
    range r7 = -3 < range() <= 7;
    BOOST_TEST(r7.start() == -2);
    BOOST_TEST(r7.stride() == 1);
    BOOST_TEST(r7.finish() == 8);
  }

  // arithmetic operators
  {
    range r8 = range(0,5) + 2;
    BOOST_TEST(r8.start() == 2);
    BOOST_TEST(r8.stride() == 1);
    BOOST_TEST(r8.finish() == 7);
  }

  {
    range r9 = range(0,5) - 2;
    BOOST_TEST(r9.start() == -2);
    BOOST_TEST(r9.stride() == 1);
    BOOST_TEST(r9.finish() == 3);
  }

  return boost::exit_success;
}
