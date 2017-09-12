//
// resize.cpp - Test of resizing multi_arrays
//

#include "boost/multi_array.hpp"
#include <iostream>
using namespace std;

template <typename Array>
void print(std::ostream& os, const Array& A)
{
  typename Array::const_iterator i;
  os << "[";
  for (i = A.begin(); i != A.end(); ++i) {
    print(os, *i);
    if (boost::next(i) != A.end())
      os << ',';
  }
  os << "]";
}

void print(std::ostream& os, const int& x)
{
  os << x;
}

int main() {

  typedef boost::multi_array<int,3> marray;


  int A_data[] = {
    0,1,2,3,
    4,5,6,7,
    8,9,10,11,

    12,13,14,15,
    16,17,18,19,
    20,21,22,23
  };


  marray A(boost::extents[2][3][4]);

  A.assign(A_data,A_data+(2*3*4));

  A.resize(boost::extents[4][3][2]);
  
  int A_resize[] = {
    0,1,
    4,5,
    8,9,

    12,13,
    16,17,
    20,21,

    0,0,
    0,0,
    0,0,

    0,0,
    0,0,
    0,0
  };

  assert(std::equal(A_resize,A_resize+(4*3*2),A.data()));

}
