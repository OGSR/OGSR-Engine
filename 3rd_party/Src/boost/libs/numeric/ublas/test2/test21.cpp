//
//  Copyright (c) 2000-2002
//  Joerg Walter, Mathias Koch
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  The authors make no representations
//  about the suitability of this software for any purpose.
//  It is provided "as is" without express or implied warranty.
//
//  The authors gratefully acknowledge the support of
//  GeNeSys mbH & Co. KG in producing this work.
//

#ifdef BOOST_MSVC

#pragma warning (disable: 4355)
#pragma warning (disable: 4503)
#pragma warning (disable: 4786)

#endif

#include <iostream>

#include <boost/numeric/ublas/config.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/numeric/ublas/blas.hpp>

#include "test2.hpp"

template<class V, int N>
void test_blas_1<V, N>::operator () () {
    try {
        value_type t;
        real_type n;
        V v1 (N), v2 (N);

        // _asum
        initialize_vector (v1);
        n = ublas::blas_1::asum (v1);
        std::cout << "asum (v1) = " << n << std::endl;

        // _amax
        initialize_vector (v1);
        n = ublas::blas_1::amax (v1);
        std::cout << "amax (v1) = " << n << std::endl;

        // _nrm2
        initialize_vector (v1);
        n = ublas::blas_1::nrm2 (v1);
        std::cout << "nrm2 (v1) = " << n << std::endl;

        // _dot
        // _dotu
        // _dotc
        initialize_vector (v1);
        initialize_vector (v2);
        t = ublas::blas_1::dot (v1, v2);
        std::cout << "dot (v1, v2) = " << t << std::endl;
        t = ublas::blas_1::dot (ublas::conj (v1), v2);
        std::cout << "dot (conj (v1), v2) = " << t << std::endl;

        // _copy
        initialize_vector (v2);
        ublas::blas_1::copy (v1, v2);
        std::cout << "copy (v1, v2) = " << v1 << std::endl;

        // _swap
        initialize_vector (v1);
        initialize_vector (v2);
        ublas::blas_1::swap (v1, v2);
        std::cout << "swap (v1, v2) = " << v1 << " " << v2 << std::endl;

        // _scal
        // csscal
        // zdscal
        initialize_vector (v1);
        ublas::blas_1::scal (v1, value_type (1));
        std::cout << "scal (v1, 1) = " << v1 << std::endl;

        // _axpy
        initialize_vector (v1);
        initialize_vector (v2);
        ublas::blas_1::axpy (v1, value_type (1), v2);
        std::cout << "axpy (v1, 1, v2) = " << v1 << std::endl;

        // _rot
        initialize_vector (v1);
        initialize_vector (v2);
        ublas::blas_1::rot (value_type (1), v1, value_type (1), v2);
        std::cout << "rot (1, v1, 1, v2) = " << v1 << " " << v2 << std::endl;
    }
    catch (std::exception &e) {
        std::cout << e.what () << std::endl;
    }
    catch (...) {
        std::cout << "unknown exception" << std::endl;
    }
}

#ifdef USE_FLOAT
template struct test_blas_1<ublas::vector<float>, 3>;
#endif

#ifdef USE_DOUBLE
template struct test_blas_1<ublas::vector<double>, 3>;
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
template struct test_blas_1<ublas::vector<std::complex<float> >, 3>;
#endif

#ifdef USE_DOUBLE
template struct test_blas_1<ublas::vector<std::complex<double> >, 3>;
#endif
#endif
