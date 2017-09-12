// 
// index_bases - test of the index_base modifying facilities.
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"
#include <vector>
#include <iostream>
int
test_main(int,char*[])
{
  typedef boost::multi_array<double, 3> array;
  typedef array::array_view<3>::type array_view;

  typedef array::extent_range erange;
  typedef array::index_range irange;

  array::extent_gen extents;
  array::index_gen indices;

  // Construct with nonzero bases
  {

    array A(extents[erange(1,4)][erange(2,5)][erange(3,6)]);
    array_view E = A[indices[irange(1,2)][irange(1,2)][irange(1,2)]];

  }
  return boost::exit_success;
}
