//
// fail_citerator.cpp -
//   const_iterator/iterator conversion  test
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"


int test_main(int,char*[]) {
  typedef boost::multi_array<int,3> array;

  typedef array::iterator iterator1;
  typedef array::const_iterator citerator1;

  // ILLEGAL conversion from const_iterator to iterator
  iterator1 in = citerator1();

  return boost::exit_success;
}
