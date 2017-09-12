/* Boost test/det.hpp
 * test protected and unprotected rounding on an unstable determinant
 *
 * Copyright Guillaume Melquiond 2002-2003
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation.
 *
 * None of the above authors nor Polytechnic University make any
 * representation about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * $Id: det.cpp,v 1.3 2003/02/05 17:34:36 gmelquio Exp $
 */

#include <boost/numeric/interval.hpp>
#include <boost/test/minimal.hpp>

#define size 8

template<class I>
void det(I (&mat)[size][size]) {
  for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++)
      mat[i][j] = I(1) / I(i + j + 1);

  for(int i = 0; i < size - 1; i++) {
    int m = i, n = i;
    typename I::base_type v = 0;
    for(int a = i; a < size; a++)
      for(int b = i; b < size; b++) {
        typename I::base_type  w = abs(mat[a][b]).lower();
        if (w > v) { m = a; n = b; v = w; }
      }
    if (n != i)
      for(int a = 0; a < size; a++) {
        I t = mat[a][n];
        mat[a][n] = mat[a][i];
        mat[a][i] = t;
      }
    if (m != i)
      for(int b = i; b < size; b++) {
        I t = mat[m][b];
        mat[m][b] = mat[m][i];
        mat[m][i] = t;
      }
    if (((m + n) & 1) == 1) { };
    I c = mat[i][i];
    for(int j = i + 1; j < size; j++) {
      I f = mat[j][i] / c;
      for(int k = i; k < size; k++)
        mat[j][k] -= f * mat[i][k];
    }
    if (in_zero(c)) return;
  }
}

namespace my_namespace {

using namespace boost;
using namespace numeric;
using namespace interval_lib;

template<class T>
struct variants {
  typedef interval<T> I_op;
  typedef typename change_rounding<I_op, save_state<rounded_arith_std<T> > >::type I_sp;
  typedef typename unprotect<I_op>::type I_ou;
  typedef typename unprotect<I_sp>::type I_su;
  typedef T type;
};

}

template<class T>
bool test() {
  typedef my_namespace::variants<double> types;
  types::I_op mat_op[size][size];
  types::I_sp mat_sp[size][size];
  types::I_ou mat_ou[size][size];
  types::I_su mat_su[size][size];
  det(mat_op);
  det(mat_sp);
  { types::I_op::traits_type::rounding rnd; det(mat_ou); }
  { types::I_sp::traits_type::rounding rnd; det(mat_su); }
  for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++) {
      typedef types::I_op I;
      I d_op = mat_op[i][j];
      I d_sp = mat_sp[i][j];
      I d_ou = mat_ou[i][j];
      I d_su = mat_su[i][j];
      if (!(equal(d_op, d_sp) && equal(d_sp, d_ou) && equal(d_ou, d_su)))
        return false;
    }
  return true;
}

int test_main(int, char *[]) {
  BOOST_TEST(test<float>());
  BOOST_TEST(test<double>());
  BOOST_TEST(test<long double>());
  return 0;
}
