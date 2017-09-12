//
// test out my new storage_order stuff
//

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/multi_array/storage_order.hpp"

int 
test_main(int,char*[]) {

  using namespace boost;

  array<std::size_t,5> c_ordering = {{4,3,2,1,0}};;
  array<std::size_t,5> fortran_ordering = {{0,1,2,3,4}};
  array<bool,5> ascending = {{true,true,true,true,true}};
  general_storage_order<5> c_storage(c_ordering.begin(),
                                     ascending.begin());
  general_storage_order<5> fortran_storage(fortran_ordering.begin(),
                                           ascending.begin());
 
  BOOST_TEST(c_storage == (general_storage_order<5>) c_storage_order());
  BOOST_TEST(fortran_storage ==
             (general_storage_order<5>) fortran_storage_order());

  return boost::exit_success;
}
