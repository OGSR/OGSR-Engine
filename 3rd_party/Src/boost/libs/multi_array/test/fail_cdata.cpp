// 
// fail_cdata.cpp
//   Testing data() member function constness.
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
  int data[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,
                 14,15,16,17,18,19,20,21,22,23};
  const int data_size = 24;
  array sma(sma_dims);
  sma.assign(data,data+data_size);
  const array& csma = sma;

  // FAIL! data() returns a const int*
  int* cdptr = csma.data();
  (void)cdptr; // suppress compiler warnings;
  return boost::exit_success;
}
