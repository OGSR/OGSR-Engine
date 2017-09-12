//
// idxset1.cpp - testing the code for index_gen
//

#include "boost/multi_array/index_gen.hpp"
#include "boost/multi_array/index_range.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"
#include <cstddef>


template <int NumRanges, int NumDims>
void check(const boost::detail::multi_array::
           index_gen<NumRanges,NumDims>&) { }

bool operator==(const boost::detail::multi_array::
                index_range<int,std::size_t>& lhs,
                const boost::detail::multi_array::
                index_range<int,std::size_t>& rhs) {
  return lhs.start_ == rhs.start_ &&
    lhs.finish_ == rhs.finish_ &&
    lhs.stride_ == rhs.stride_ &&
    lhs.degenerate_ == rhs.degenerate_;
}

int
test_main(int,char*[])
{
  typedef boost::detail::multi_array::index_range<int,std::size_t> range;

  boost::detail::multi_array::index_gen<0,0> indices;


  check<1,1>(indices[range()]);
  check<2,2>(indices[range()][range()]);
  check<3,3>(indices[range()][range()][range()]);

  check<1,0>(indices[0]);
  check<2,0>(indices[0][0]);
  check<2,1>(indices[range()][0]);
  check<2,1>(indices[0][range()]);
  check<3,0>(indices[0][0][0]);
  check<3,1>(indices[range()][0][0]);
  check<3,1>(indices[0][range()][0]);
  check<3,1>(indices[0][0][range()]);
  check<3,2>(indices[range()][range()][0]);
  check<3,2>(indices[range()][0][range()]);
  check<3,2>(indices[0][range()][range()]);

  {
    boost::detail::multi_array::index_gen<3,3> is1 =
      indices[range(0,1,2)][range(1,2,3)][range(2,3,4)];
    BOOST_TEST(is1.ranges_[0] == range(0,1,2));
    BOOST_TEST(is1.ranges_[1] == range(1,2,3));
    BOOST_TEST(is1.ranges_[2] == range(2,3,4));
  }

  {
    boost::detail::multi_array::index_gen<3,2> is2 = 
      indices[range(0,1,2)][2][range(2,3,4)];
    BOOST_TEST(is2.ranges_[0] == range(0,1,2));
    BOOST_TEST(is2.ranges_[1] == range(2));
    BOOST_TEST(is2.ranges_[1].is_degenerate());
    BOOST_TEST(is2.ranges_[2] == range(2,3,4));
  }

  return boost::exit_success;
}

