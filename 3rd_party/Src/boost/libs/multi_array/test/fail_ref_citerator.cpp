//
// fail_ref_citerator.cpp
//   const_iterator/iterator conversion  test
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"


int test_main(int,char*[]) {
  typedef boost::multi_array_ref<int,3> array_ref;

  typedef array_ref::iterator iterator1;
  typedef array_ref::const_iterator citerator1;

  // FAIL! ILLEGAL conversion from const_iterator to iterator
  iterator1 in = citerator1();

  return boost::exit_success;
}
