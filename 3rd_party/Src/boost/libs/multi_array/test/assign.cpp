// 
// assign.cpp - Test out operator=() on the different types
// 
//
#include "generative_tests.hpp"
#include "boost/array.hpp"
#include "boost/multi_array.hpp"
#include "boost/cstdlib.hpp"
#include <algorithm>
#include <iostream>

bool equal(const int& a, const int& b)
{
  return a == b;
}

template <typename ArrayA, typename ArrayB>
bool equal(const ArrayA& A, const ArrayB& B)
{
  typename ArrayA::const_iterator ia;
  typename ArrayB::const_iterator ib = B.begin();
  for (ia = A.begin(); ia != A.end(); ++ia, ++ib)
    if (!equal(*ia, *ib))
      return false;
  return true;
}


template <typename Array>
void access(Array& A, const mutable_array_tag&) {
  assign(A);
  typedef boost::multi_array<int,3> array3;

  int insert[] = {
    99,98,97,96,
    95,94,93,92,
    91,90,89,88,

    87,86,85,84,
    83,82,81,80,
    79,78,77,76
  };
  const int insert_size = 2*3*4;
  array3 filler(boost::extents[2][3][4]);
  filler.assign(insert,insert+insert_size);


  A = filler;

  BOOST_TEST(equal(A,filler));
  ++tests_run;
}

template <typename Array>
void access(Array&, const const_array_tag&) {
}


int test_main(int,char*[]) {
  return run_generative_tests();
}
