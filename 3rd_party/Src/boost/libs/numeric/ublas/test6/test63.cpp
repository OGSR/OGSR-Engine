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
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "test6.hpp"

// Test matrix expression templates
template<class M, int N>
struct test_my_matrix {
    typedef typename M::value_type value_type;

    template<class MP>
    void operator () (MP &m1, MP &m2, MP &m3) const {
        try {
            value_type t;

            // Copy and swap
            initialize_matrix (m1, ublas::lower_tag ());
            initialize_matrix (m2, ublas::lower_tag ());
            m1 = m2;
            std::cout << "m1 = m2 = " << m1 << std::endl;
            m1.assign_temporary (m2);
            std::cout << "m1.assign_temporary (m2) = " << m1 << std::endl;
            m1.swap (m2);
            std::cout << "m1.swap (m2) = " << m1 << " " << m2 << std::endl;

            // Unary matrix operations resulting in a matrix
            initialize_matrix (m1, ublas::lower_tag ());
            m2 = - m1;
            std::cout << "- m1 = " << m2 << std::endl;
            m2 = ublas::conj (m1);
            std::cout << "conj (m1) = " << m2 << std::endl;

            // Binary matrix operations resulting in a matrix
            initialize_matrix (m1, ublas::lower_tag ());
            initialize_matrix (m2, ublas::lower_tag ());
            m3 = m1 + m2;
            std::cout << "m1 + m2 = " << m3 << std::endl;
            m3 = m1 - m2;
            std::cout << "m1 - m2 = " << m3 << std::endl;

            // Scaling a matrix
            t = N;
            initialize_matrix (m1, ublas::lower_tag ());
            m2 = value_type (1.) * m1;
            std::cout << "1. * m1 = " << m2 << std::endl;
            m2 = t * m1;
            std::cout << "N * m1 = " << m2 << std::endl;
            initialize_matrix (m1, ublas::lower_tag ());
            m2 = m1 * value_type (1.);
            std::cout << "m1 * 1. = " << m2 << std::endl;
            m2 = m1 * t;
            std::cout << "m1 * N = " << m2 << std::endl;

            // Some assignments
            initialize_matrix (m1, ublas::lower_tag ());
            initialize_matrix (m2, ublas::lower_tag ());
#ifdef BOOST_UBLAS_USE_ET
            m2 += m1;
            std::cout << "m2 += m1 = " << m2 << std::endl;
            m2 -= m1;
            std::cout << "m2 -= m1 = " << m2 << std::endl;
#else
            m2 = m2 + m1;
            std::cout << "m2 += m1 = " << m2 << std::endl;
            m2 = m2 - m1;
            std::cout << "m2 -= m1 = " << m2 << std::endl;
#endif
            m1 *= value_type (1.);
            std::cout << "m1 *= 1. = " << m1 << std::endl;
            m1 *= t;
            std::cout << "m1 *= N = " << m1 << std::endl;

            // Transpose
            initialize_matrix (m1, ublas::lower_tag ());
            m2 = ublas::trans (m1);
            std::cout << "trans (m1) = " << m2 << std::endl;

            // Hermitean
            initialize_matrix (m1, ublas::lower_tag ());
            m2 = ublas::herm (m1);
            std::cout << "herm (m1) = " << m2 << std::endl;

            // Matrix multiplication
            initialize_matrix (m1, ublas::lower_tag ());
            initialize_matrix (m2, ublas::lower_tag ());
            m3 = ublas::prod (m1, m2);
            std::cout << "prod (m1, m2) = " << m3 << std::endl;
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
    }
    void operator () () const {
        try {
            M m1 (N, N), m2 (N, N), m3 (N, N);
            (*this) (m1, m2, m3);

#ifdef USE_RANGE
            ublas::matrix_range<M> mr1 (m1, ublas::range (0, N), ublas::range (0, N)),
                                   mr2 (m2, ublas::range (0, N), ublas::range (0, N)),
                                   mr3 (m3, ublas::range (0, N), ublas::range (0, N));
            (*this) (mr1, mr2, mr3);
#endif

#ifdef USE_SLICE
            ublas::matrix_slice<M> ms1 (m1, ublas::slice (0, 1, N), ublas::slice (0, 1, N)),
                                   ms2 (m2, ublas::slice (0, 1, N), ublas::slice (0, 1, N)),
                                   ms3 (m3, ublas::slice (0, 1, N), ublas::slice (0, 1, N));
            (*this) (ms1, ms2, ms3);
#endif
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
    }
    void operator () (int) const {
#ifdef USE_ADAPTOR
        try {
            M m1 (N, N), m2 (N, N), m3 (N, N);
            ublas::symmetric_adaptor<M> sam1 (m1), sam2 (m2), sam3 (m3);
            (*this) (sam1, sam2, sam3);

#ifdef USE_RANGE
            ublas::matrix_range<ublas::symmetric_adaptor<M> > mr1 (sam1, ublas::range (0, N), ublas::range (0, N)),
                                                              mr2 (sam2, ublas::range (0, N), ublas::range (0, N)),
                                                              mr3 (sam3, ublas::range (0, N), ublas::range (0, N));
            (*this) (mr1, mr2, mr3);
#endif

#ifdef USE_SLICE
            ublas::matrix_slice<ublas::symmetric_adaptor<M> > ms1 (sam1, ublas::slice (0, 1, N), ublas::slice (0, 1, N)),
                                                              ms2 (sam2, ublas::slice (0, 1, N), ublas::slice (0, 1, N)),
                                                              ms3 (sam3, ublas::slice (0, 1, N), ublas::slice (0, 1, N));
            (*this) (ms1, ms2, ms3);
#endif
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
#endif
    }
};

// Test matrix
void test_matrix () {
    std::cout << "test_matrix" << std::endl;

#ifdef USE_BOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "float, bounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, ublas::bounded_array<float, 3 * 3> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, ublas::bounded_array<float, 3 * 3> >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "double, bounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, ublas::bounded_array<double, 3 * 3> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, ublas::bounded_array<double, 3 * 3> >, 3 > () (0);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, bounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, ublas::bounded_array<std::complex<float>, 3 * 3> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, ublas::bounded_array<std::complex<float>, 3 * 3> >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, bounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, ublas::bounded_array<std::complex<double>, 3 * 3> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, ublas::bounded_array<std::complex<double>, 3 * 3> >, 3 > () (0);
#endif
#endif
#endif

#ifdef USE_UNBOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "float, unbounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, ublas::unbounded_array<float> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, ublas::unbounded_array<float> >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "double, unbounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, ublas::unbounded_array<double> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, ublas::unbounded_array<double> >, 3 > () (0);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, unbounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, ublas::unbounded_array<std::complex<float> > >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, ublas::unbounded_array<std::complex<float> > >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, unbounded_array" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, ublas::unbounded_array<std::complex<double> > >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, ublas::unbounded_array<std::complex<double> > >, 3 > () (0);
#endif
#endif
#endif

#ifdef USE_STD_VECTOR
#ifdef USE_FLOAT
    std::cout << "float, std::vector" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, std::vector<float> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<float, ublas::lower, ublas::row_major, std::vector<float> >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "double, std::vector" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, std::vector<double> >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<double, ublas::lower, ublas::row_major, std::vector<double> >, 3 > () (0);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, std::vector" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, std::vector<std::complex<float> > >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<float>, ublas::lower, ublas::row_major, std::vector<std::complex<float> > >, 3 > () (0);
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, std::vector" << std::endl;
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, std::vector<std::complex<double> > >, 3 > () ();
    test_my_matrix<ublas::symmetric_matrix<std::complex<double>, ublas::lower, ublas::row_major, std::vector<std::complex<double> > >, 3 > () (0);
#endif
#endif
#endif
}


