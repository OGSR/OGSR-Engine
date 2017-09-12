//
// fail_criterator.cpp
//   const_reverse_iterator/reverse_iterator conversion  test
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"


int test_main(int,char*[]) {
    typedef boost::multi_array<int,3> array;

    typedef array::reverse_iterator riterator1;
    typedef array::const_reverse_iterator criterator1;

    // ILLEGAL conversion from const_reverse_iterator to reverse_iterator
    riterator1 in = criterator1();

    return boost::exit_success;
}
