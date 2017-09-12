// foreach_test.cpp
// Let's see if this stuff works

#include "boost/multi_array.hpp"
#include "for_each.hpp"
#include <algorithm>

struct times_five {
  double operator()(const int& val) { return val*5.0; }
};


int main() {

  typedef boost::multi_array<double,2> array;

  double data[] = {
    1.0, 2.0, 3.0,
    4.0, 5.0, 6.0,
    7.0, 8.0, 9.0
  };
  const int data_size=9;

  array A(boost::extents[3][3]);
  A.assign(data,data+data_size);

#if 0
  std::copy(A.data(),A.data()+A.num_elements(),
            std::ostream_iterator<double>(std::cout,","));

  std::cout << "\n";
#endif
  for_each(A,times_five());

#if 0  
  std::copy(A.data(),A.data()+A.num_elements(),
            std::ostream_iterator<double>(std::cout,","));
 
  std::cout << "\n";
#endif
  return 0;
}
