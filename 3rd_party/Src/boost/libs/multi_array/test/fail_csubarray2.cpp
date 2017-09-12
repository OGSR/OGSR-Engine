// 
// fail_csubarray2.cpp
//   Testing constness of subarray operations.
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"

int
test_main(int,char*[])
{
  const int ndims=3;
  typedef boost::multi_array<int,ndims> array;

  boost::array<array::size_type,ndims> sma_dims = {{2,3,4}};
  array sma(sma_dims);

  int num = 0;
  for (array::index i = 0; i != 2; ++i)
    for (array::index j = 0; j != 3; ++j)
      for (array::index k = 0; k != 4; ++k)
        sma[i][j][k] = num++;

  const array& sma_const = sma;

  // FAIL! preserve constness.
  array::subarray<ndims-1>::type sba = sma_const[0];

  return boost::exit_success;
}
