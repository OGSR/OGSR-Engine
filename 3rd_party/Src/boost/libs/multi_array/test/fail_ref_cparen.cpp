// 
// fail_ref_cparen.cpp
//   Testing const operator() constness. 
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"

int
test_main(int,char*[])
{
  const int ndims=3;
  typedef boost::multi_array_ref<int,ndims> array_ref;

  boost::array<array_ref::size_type,ndims> sma_dims = {{2,3,4}};

  int data[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,
                 14,15,16,17,18,19,20,21,22,23};

  array_ref sma(data,sma_dims);

  const array_ref& csma = sma;
  boost::array<array_ref::index,ndims> indices = {{0,0,0}};

  // FAIL! cannot assign to a const multi_array_ref
  csma(indices) = 5;

  return boost::exit_success;
}
