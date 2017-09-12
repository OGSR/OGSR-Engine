// 
// fail_ref_cview.cpp
//   ensure const_array_view doesn't allow element assignment.
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"
#include "boost/type.hpp"

int
test_main(int,char*[])
{
  const int ndims=3;
  typedef boost::multi_array_ref<int,ndims> array_ref;

  boost::array<array_ref::size_type,ndims> sma_dims = {{2,3,4}};

  int data[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,
                 14,15,16,17,18,19,20,21,22,23};

  array_ref sma(data,sma_dims);

  //
  // subarray dims:
  // [base,stride,bound)
  // [0,1,2), [1,1,3), [0,2,4) 
  // 
  
  const array_ref& csma = sma;
  typedef array_ref::index_range range;
  array_ref::index_gen indices;
  array_ref::const_array_view<ndims>::type csma2 =
    csma[indices[range(0,2)][range(1,3)][range(0,4,2)]];

  boost::array<array_ref::index,3> elmt = {{0,0,0}};

  csma2(elmt) = 5; // FAIL! csma is read only
  
  return boost::exit_success;
}







