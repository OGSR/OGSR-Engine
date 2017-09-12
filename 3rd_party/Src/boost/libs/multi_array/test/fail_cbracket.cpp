// 
// fail_cbracket.cpp - 
//   checking constness of  const operator[].
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
  

  const array& csma = sma;

  // FAIL! cannot assign to csma.
  csma[0][0][0] = 5;

  return boost::exit_success;
}
