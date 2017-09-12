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

template<class M, int N>
void test_blas_3<M, N>::operator () () {
    try {
        M m1 (N, N), m2 (N, N), m3 (N, N);

        // _t_mm
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), m2, m1);
        std::cout << "tmm (m1, 1, m2, m1) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), m2, ublas::trans (m1));
        std::cout << "tmm (m1, 1, m2, trans (m1)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::trans (m2), m1);
        std::cout << "tmm (m1, 1, trans (m2), m1) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::trans (m2), ublas::trans (m1));
        std::cout << "tmm (m1, 1, trans (m2), trans (m1)) = " << m1 << std::endl;
#ifdef USE_STD_COMPLEX
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), m2, ublas::herm (m1));
        std::cout << "tmm (m1, 1, m2, herm (m1)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::herm (m2), m1);
        std::cout << "tmm (m1, 1, herm (m2), m1) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::trans (m2), ublas::herm (m1));
        std::cout << "tmm (m1, 1, trans (m2), herm (m1)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::herm (m2), ublas::trans (m1));
        std::cout << "tmm (m1, 1, herm (m2), trans (m1)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::tmm (m1, value_type (1), ublas::herm (m2), ublas::herm (m1));
        std::cout << "tmm (m1, 1, herm (m2), herm (m1)) = " << m1 << std::endl;
#endif

        // _t_sm
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::lower_tag ());
        initialize_matrix (m3);
        ublas::blas_3::tsm (m1, value_type (1), m2, ublas::lower_tag ());
        std::cout << "tsm (m1, 1, m2) = " << m1 << " " << ublas::prod (m2, m1) - value_type (1) * m3 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::upper_tag ());
        ublas::blas_3::tsm (m1, value_type (1), ublas::trans (m2), ublas::lower_tag ());
        std::cout << "tsm (m1, 1, trans (m2)) = " << m1 << " " << ublas::prod (ublas::trans (m2), m1) - value_type (1) * m3 << std::endl;
#ifdef USE_STD_COMPLEX
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::upper_tag ());
        ublas::blas_3::tsm (m1, value_type (1), ublas::herm (m2), ublas::lower_tag ());
        std::cout << "tsm (m1, 1, herm (m2)) = " << m1 << " " << ublas::prod (ublas::herm (m2), m1) - value_type (1) * m3 << std::endl;
#endif
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::upper_tag ());
        ublas::blas_3::tsm (m1, value_type (1), m2, ublas::upper_tag ());
        std::cout << "tsm (m1, 1, m2) = " << m1 << " " << ublas::prod (m2, m1) - value_type (1) * m3 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::lower_tag ());
        ublas::blas_3::tsm (m1, value_type (1), ublas::trans (m2), ublas::upper_tag ());
        std::cout << "tsm (m1, 1, trans (m2)) = " << m1 << " " << ublas::prod (ublas::trans (m2), m1) - value_type (1) * m3 << std::endl;
#ifdef USE_STD_COMPLEX
        initialize_matrix (m1);
        initialize_matrix (m2, ublas::lower_tag ());
        ublas::blas_3::tsm (m1, value_type (1), ublas::herm (m2), ublas::upper_tag ());
        std::cout << "tsm (m1, 1, herm (m2)) = " << m1 << " " << ublas::prod (ublas::herm (m2), m1) - value_type (1) * m3 << std::endl;
#endif

        // _g_mm
        // _s_mm
        // _h_mm
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), m2, m3);
        std::cout << "gmm (m1, 1, 1, m2, m3) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::trans (m2), m3);
        std::cout << "gmm (m1, 1, 1, trans (m2), m3) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), m2, ublas::trans (m3));
        std::cout << "gmm (m1, 1, 1, m2, trans (m3)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::trans (m2), ublas::trans (m3));
        std::cout << "gmm (m1, 1, 1, trans (m2), trans (m3)) = " << m1 << std::endl;
#ifdef USE_STD_COMPLEX
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::herm (m2), m3);
        std::cout << "gmm (m1, 1, 1, herm (m2), m3) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), m2, ublas::herm (m3));
        std::cout << "gmm (m1, 1, 1, m2, herm (m3)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::herm (m2), ublas::trans (m3));
        std::cout << "gmm (m1, 1, 1, herm (m2), trans (m3)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::trans (m2), ublas::herm (m3));
        std::cout << "gmm (m1, 1, 1, trans (m2), herm (m3)) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::gmm (m1, value_type (1), value_type (1), ublas::herm (m2), ublas::herm (m3));
        std::cout << "gmm (m1, 1, 1, herm (m2), herm (m3)) = " << m1 << std::endl;
#endif

        // s_rk
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::srk (m1, value_type (1), value_type (1), m2);
        std::cout << "srk (m1, 1, 1, m2) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::srk (m1, value_type (1), value_type (1), ublas::trans (m2));
        std::cout << "srk (m1, 1, 1, trans (m2)) = " << m1 << std::endl;

#ifdef USE_STD_COMPLEX
        // h_rk
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::hrk (m1, value_type (1), value_type (1), m2);
        std::cout << "hrk (m1, 1, 1, m2) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        ublas::blas_3::hrk (m1, value_type (1), value_type (1), ublas::herm (m2));
        std::cout << "hrk (m1, 1, 1, herm (m2)) = " << m1 << std::endl;
#endif

        // s_r2k
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::sr2k (m1, value_type (1), value_type (1), m2, m3);
        std::cout << "sr2k (m1, 1, 1, m2, m3) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::sr2k (m1, value_type (1), value_type (1), ublas::trans (m2), ublas::trans (m3));
        std::cout << "sr2k (m1, 1, 1, trans (m2), trans (m3)) = " << m1 << std::endl;

#ifdef USE_STD_COMPLEX
        // h_r2k
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::hr2k (m1, value_type (1), value_type (1), m2, m3);
        std::cout << "hr2k (m1, 1, 1, m2, m3) = " << m1 << std::endl;
        initialize_matrix (m1);
        initialize_matrix (m2);
        initialize_matrix (m3);
        ublas::blas_3::hr2k (m1, value_type (1), value_type (1), ublas::herm (m2), ublas::herm (m3));
        std::cout << "hr2k (m1, 1, 1, herm (m2), herm (m3)) = " << m1 << std::endl;
#endif
    }
    catch (std::exception &e) {
        std::cout << e.what () << std::endl;
    }
    catch (...) {
        std::cout << "unknown exception" << std::endl;
    }
}

#ifdef USE_FLOAT
template struct test_blas_3<ublas::matrix<float>, 3>;
#endif

#ifdef USE_DOUBLE
template struct test_blas_3<ublas::matrix<double>, 3>;
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
template struct test_blas_3<ublas::matrix<std::complex<float> >, 3>;
#endif

#ifdef USE_DOUBLE
template struct test_blas_3<ublas::matrix<std::complex<double> >, 3>;
#endif
#endif
