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
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "test3.hpp"

// Test vector expression templates
template<class V, int N>
struct test_my_vector {
    typedef typename V::value_type value_type;
    typedef typename V::size_type size_type;
    typedef typename ublas::type_traits<value_type>::real_type real_type;

    template<class VP>
    void operator () (VP &v1, VP &v2, VP &v3) const {
        try {
            value_type t;
            size_type i;
            real_type n;

            // Copy and swap
            initialize_vector (v1);
            initialize_vector (v2);
            v1 = v2;
            std::cout << "v1 = v2 = " << v1 << std::endl;
            v1.assign_temporary (v2);
            std::cout << "v1.assign_temporary (v2) = " << v1 << std::endl;
            v1.swap (v2);
            std::cout << "v1.swap (v2) = " << v1 << " " << v2 << std::endl;

            // Unary vector operations resulting in a vector
            initialize_vector (v1);
            v2 = - v1;
            std::cout << "- v1 = " << v2 << std::endl;
            v2 = ublas::conj (v1);
            std::cout << "conj (v1) = " << v2 << std::endl;

            // Binary vector operations resulting in a vector
            initialize_vector (v1);
            initialize_vector (v2);
            initialize_vector (v3);
            v3 = v1 + v2;
            std::cout << "v1 + v2 = " << v3 << std::endl;

            v3 = v1 - v2;
            std::cout << "v1 - v2 = " << v3 << std::endl;

            // Scaling a vector
            t = N;
            initialize_vector (v1);
            v2 = value_type (1.) * v1;
            std::cout << "1. * v1 = " << v2 << std::endl;
            v2 = t * v1;
            std::cout << "N * v1 = " << v2 << std::endl;
            initialize_vector (v1);
            v2 = v1 * value_type (1.);
            std::cout << "v1 * 1. = " << v2 << std::endl;
            v2 = v1 * t;
            std::cout << "v1 * N = " << v2 << std::endl;

            // Some assignments
            initialize_vector (v1);
            initialize_vector (v2);
#ifdef BOOST_UBLAS_USE_ET
            v2 += v1;
            std::cout << "v2 += v1 = " << v2 << std::endl;
            v2 -= v1;
            std::cout << "v2 -= v1 = " << v2 << std::endl;
#else
            v2 = v2 + v1;
            std::cout << "v2 += v1 = " << v2 << std::endl;
            v2 = v2 - v1;
            std::cout << "v2 -= v1 = " << v2 << std::endl;
#endif
            v1 *= value_type (1.);
            std::cout << "v1 *= 1. = " << v1 << std::endl;
            v1 *= t;
            std::cout << "v1 *= N = " << v1 << std::endl;

            // Unary vector operations resulting in a scalar
            initialize_vector (v1);
            t = ublas::sum (v1);
            std::cout << "sum (v1) = " << t << std::endl;
            n = ublas::norm_1 (v1);
            std::cout << "norm_1 (v1) = " << n << std::endl;
            n = ublas::norm_2 (v1);
            std::cout << "norm_2 (v1) = " << n << std::endl;
            n = ublas::norm_inf (v1);
            std::cout << "norm_inf (v1) = " << n << std::endl;

            i = ublas::index_norm_inf (v1);
            std::cout << "index_norm_inf (v1) = " << i << std::endl;

            // Binary vector operations resulting in a scalar
            initialize_vector (v1);
            initialize_vector (v2);
            t = ublas::inner_prod (v1, v2);
            std::cout << "inner_prod (v1, v2) = " << t << std::endl;
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
            V v1 (N, N), v2 (N, N), v3 (N, N);
            (*this) (v1, v2, v3);

#ifdef USE_RANGE
            ublas::vector_range<V> vr1 (v1, ublas::range (0, N)),
                                   vr2 (v2, ublas::range (0, N)),
                                   vr3 (v3, ublas::range (0, N));
            (*this) (vr1, vr2, vr3);
#endif

#ifdef USE_SLICE
            ublas::vector_slice<V> vs1 (v1, ublas::slice (0, 1, N)),
                                   vs2 (v2, ublas::slice (0, 1, N)),
                                   vs3 (v3, ublas::slice (0, 1, N));
            (*this) (vs1, vs2, vs3);
#endif
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
    }
};

// Test vector
void test_vector () {
    std::cout << "test_vector" << std::endl;

#ifdef USE_SPARSE_VECTOR
#ifdef USE_MAP_ARRAY
#ifdef USE_FLOAT
    std::cout << "float, map_array" << std::endl;
    test_my_vector<ublas::sparse_vector<float, ublas::map_array<std::size_t, float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, map_array" << std::endl;
    test_my_vector<ublas::sparse_vector<double, ublas::map_array<std::size_t, double> >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, map_array" << std::endl;
    test_my_vector<ublas::sparse_vector<std::complex<float>, ublas::map_array<std::size_t, std::complex<float> > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, map_array" << std::endl;
    test_my_vector<ublas::sparse_vector<std::complex<double>, ublas::map_array<std::size_t, std::complex<double> > >, 3 > () ();
#endif
#endif
#endif

#ifdef USE_STD_MAP
#ifdef USE_FLOAT
    std::cout << "float, std::map" << std::endl;
    test_my_vector<ublas::sparse_vector<float, std::map<size_t, float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, std::map" << std::endl;
    test_my_vector<ublas::sparse_vector<double, std::map<size_t, double> >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, std::map" << std::endl;
    test_my_vector<ublas::sparse_vector<std::complex<float>, std::map<size_t, std::complex<float> > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, std::map" << std::endl;
    test_my_vector<ublas::sparse_vector<std::complex<double>, std::map<size_t, std::complex<double> > > , 3 > () ();
#endif
#endif
#endif
#endif

#ifdef USE_COMPRESSED_VECTOR
#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_my_vector<ublas::compressed_vector<float>, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_my_vector<ublas::compressed_vector<double>, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_my_vector<ublas::compressed_vector<std::complex<float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>" << std::endl;
    test_my_vector<ublas::compressed_vector<std::complex<double> >, 3 > () ();
#endif
#endif
#endif

#ifdef USE_COORDINATE_VECTOR
#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_my_vector<ublas::coordinate_vector<float>, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_my_vector<ublas::coordinate_vector<double>, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_my_vector<ublas::coordinate_vector<std::complex<float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>" << std::endl;
    test_my_vector<ublas::coordinate_vector<std::complex<double> >, 3 > () ();
#endif
#endif
#endif
}

