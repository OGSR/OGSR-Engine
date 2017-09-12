// 
// index_bases - test of the index_base modifying facilities.
//

#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"
#include <vector>
#include <iostream>
int
test_main(int,char*[])
{
  typedef boost::multi_array<double, 3> array;
  typedef boost::multi_array_ref<double, 3> array_ref;
  typedef boost::const_multi_array_ref<double, 3> const_array_ref;
  typedef array::array_view<3>::type array_view;

  typedef array::size_type size_type;
  typedef array::extent_range range;
  typedef array::index_range irange;

  array::extent_gen extents;
  array::index_gen indices;

  // Construct with nonzero bases
  {

    array A(extents[range(1,4)][range(2,5)][range(3,6)]);
    array B(extents[3][3][3]);

    double ptr[27];
    array_ref
      C(ptr,extents[range(1,4)][range(2,5)][range(3,6)]);

    const_array_ref
      D(ptr,extents[range(1,4)][range(2,5)][range(3,6)]);

    array_view E = A[indices[irange()][irange()][irange()]];

    std::vector<double> vals;
    for (int i = 0; i < 27; ++i)
      vals.push_back(i);

    A.assign(vals.begin(),vals.end());
    B.assign(vals.begin(),vals.end());
    C.assign(vals.begin(),vals.end());

    boost::array<int,3> bases = { { 1, 2, 3 } };
    for (size_type a = 0; a < A.shape()[0]; ++a)
      for (size_type b = 0; b < A.shape()[1]; ++b)
        for (size_type c = 0; c < A[b].size(); ++c) {
          BOOST_TEST(A[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          BOOST_TEST(C[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          BOOST_TEST(D[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          // Test that E does not inherit A's index_base
          BOOST_TEST(E[a][b][c] == B[a][b][c]);
        }
  }

  // Reindex
  {
    typedef array::size_type size_type;
    array A(extents[3][3][3]), B(extents[3][3][3]);

    double ptr[27];
    array_ref C(ptr,extents[3][3][3]);
    const_array_ref D(ptr,extents[3][3][3]);

    array_view E = B[indices[irange()][irange()][irange()]];

    std::vector<double> vals;
    for (int i = 0; i < 27; ++i)
      vals.push_back(i);

    A.assign(vals.begin(),vals.end());
    B.assign(vals.begin(),vals.end());
    C.assign(vals.begin(),vals.end());

    boost::array<int,3> bases = { { 1, 2, 3 } };

    A.reindex(bases);
    C.reindex(bases);
    D.reindex(bases);
    E.reindex(bases);

    for (size_type a = 0; a < A.shape()[0]; ++a)
      for (size_type b = 0; b < A.shape()[1]; ++b)
        for (size_type c = 0; c < A.shape()[2]; ++c) {
          BOOST_TEST(A[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          BOOST_TEST(C[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          BOOST_TEST(D[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
          BOOST_TEST(E[a+bases[0]][b+bases[1]][c+bases[2]] == B[a][b][c]);
        }
  }

  // Set Index Base
  {
    typedef array::size_type size_type;
    array A(extents[3][3][3]), B(extents[3][3][3]);

    double ptr[27];
    array_ref C(ptr,extents[3][3][3]);
    const_array_ref D(ptr,extents[3][3][3]);

    array_view E = B[indices[irange()][irange()][irange()]];

    std::vector<double> vals;
    for (int i = 0; i < 27; ++i)
      vals.push_back(i);

    A.assign(vals.begin(),vals.end());
    B.assign(vals.begin(),vals.end());
    C.assign(vals.begin(),vals.end());

    A.reindex(1);
    C.reindex(1);
    D.reindex(1);
    E.reindex(1);

    for (size_type a = 0; a < A.shape()[0]; ++a)
      for (size_type b = 0; b < A.shape()[1]; ++b)
        for (size_type c = 0; c < A.shape()[2]; ++c) {
          BOOST_TEST(A[a+1][b+1][c+1] == B[a][b][c]);
          BOOST_TEST(C[a+1][b+1][c+1] == B[a][b][c]);
          BOOST_TEST(D[a+1][b+1][c+1] == B[a][b][c]);
          BOOST_TEST(E[a+1][b+1][c+1] == B[a][b][c]);
        }
  }

  return boost::exit_success;
}
