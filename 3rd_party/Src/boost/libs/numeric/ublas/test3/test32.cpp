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

// Test matrix & vector expression templates
template<class V, class M, int N>
struct test_my_matrix_vector {
    typedef typename V::value_type value_type;

    template<class VP, class MP>
    void operator () (VP &v1, VP &v2, MP &m1) const {
        try {
            // Rows and columns
            initialize_matrix (m1);
            for (int i = 0; i < N; ++ i) {
                v1 = ublas::row (m1, i);
                std::cout << "row (m, " << i << ") = " << v1 << std::endl;
                v1 = ublas::column (m1, i);
                std::cout << "column (m, " << i << ") = " << v1 << std::endl;
            }

            // Outer product
            initialize_vector (v1);
            initialize_vector (v2);
            m1 = ublas::outer_prod (v1, v2);
            std::cout << "outer_prod (v1, v2) = " << m1 << std::endl;

            // Matrix vector product
            initialize_matrix (m1);
            initialize_vector (v1);
            v2 = ublas::prod (m1, v1);
            std::cout << "prod (m1, v1) = " << v2 << std::endl;
            v2 = ublas::prod (v1, m1);
            std::cout << "prod (v1, m1) = " << v2 << std::endl;
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
            V v1 (N, N), v2 (N, N);
            M m1 (N, N, N * N);

            (*this) (v1, v2, m1);

            ublas::matrix_row<M> mr1 (m1, 0), mr2 (m1, N - 1);
            (*this) (mr1, mr2, m1);

            ublas::matrix_column<M> mc1 (m1, 0), mc2 (m1, N - 1);
            (*this) (mc1, mc2, m1);

#ifdef USE_RANGE_AND_SLICE
            ublas::matrix_vector_range<M> mvr1 (m1, ublas::range (0, N), ublas::range (0, N)),
                                          mvr2 (m1, ublas::range (0, N), ublas::range (0, N));
            (*this) (mvr1, mvr2, m1);

            ublas::matrix_vector_slice<M> mvs1 (m1, ublas::slice (0, 1, N), ublas::slice (0, 1, N)),
                                          mvs2 (m1, ublas::slice (0, 1, N), ublas::slice (0, 1, N));
            (*this) (mvs1, mvs2, m1);
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

// Test matrix & vector
void test_matrix_vector () {
    std::cout << "test_matrix_vector" << std::endl;

#ifdef USE_SPARSE_MATRIX
#ifdef USE_MAP_ARRAY
#ifdef USE_FLOAT
    std::cout << "float, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<float, ublas::map_array<std::size_t, float> >,
                          ublas::sparse_matrix<float, ublas::row_major, ublas::map_array<std::size_t, float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<double, ublas::map_array<std::size_t, double> >,
                          ublas::sparse_matrix<double, ublas::row_major, ublas::map_array<std::size_t, double> >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<float>, ublas::map_array<std::size_t, std::complex<float> > >,
                          ublas::sparse_matrix<std::complex<float>, ublas::row_major, ublas::map_array<std::size_t, std::complex<float> > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<double>, ublas::map_array<std::size_t, std::complex<double> > >,
                          ublas::sparse_matrix<std::complex<double>, ublas::row_major, ublas::map_array<std::size_t, std::complex<double> > >, 3 > () ();
#endif
#endif
#endif

#ifdef USE_STD_MAP
#ifdef USE_FLOAT
    std::cout << "float, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<float, std::map<std::size_t, float> >,
                          ublas::sparse_matrix<float, ublas::row_major, std::map<std::size_t, float> >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<double, std::map<std::size_t, double> >,
                          ublas::sparse_matrix<double, ublas::row_major, std::map<std::size_t, double> >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<float>, std::map<std::size_t, std::complex<float> > >,
                          ublas::sparse_matrix<std::complex<float>, ublas::row_major, std::map<std::size_t, std::complex<float> > >, 3 > () ();

    std::cout << "std::complex<double>, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<double>, std::map<std::size_t, std::complex<double> > >,
                          ublas::sparse_matrix<std::complex<double>, ublas::row_major, std::map<std::size_t, std::complex<double> > >, 3 > () ();
#endif
#endif
#endif
#endif

#ifdef USE_SPARSE_VECTOR_OF_SPARSE_VECTOR
#ifdef USE_MAP_ARRAY
#ifdef USE_FLOAT
    std::cout << "float, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<float, ublas::map_array<std::size_t, float> >,
                          ublas::sparse_vector_of_sparse_vector<float, ublas::row_major, ublas::map_array<std::size_t, ublas::map_array<std::size_t, float> > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<double, ublas::map_array<std::size_t, double> >,
                          ublas::sparse_vector_of_sparse_vector<double, ublas::row_major, ublas::map_array<std::size_t, ublas::map_array<std::size_t, double> > >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<float>, ublas::map_array<std::size_t, std::complex<float> > >,
                          ublas::sparse_vector_of_sparse_vector<std::complex<float>, ublas::row_major, ublas::map_array<std::size_t, ublas::map_array<std::size_t, std::complex<float> > > >, 3 > () ();

    std::cout << "std::complex<double>, map_array" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<double>, ublas::map_array<std::size_t, std::complex<double> > >,
                          ublas::sparse_vector_of_sparse_vector<std::complex<double>, ublas::row_major, ublas::map_array<std::size_t, ublas::map_array<std::size_t, std::complex<double> > > >, 3 > () ();
#endif
#endif
#endif

#ifdef USE_STD_MAP
#ifdef USE_FLOAT
    std::cout << "float, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<float, std::map<std::size_t, float> >,
                          ublas::sparse_vector_of_sparse_vector<float, ublas::row_major, std::map<std::size_t, std::map<std::size_t, float> > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<double, std::map<std::size_t, double> >,
                          ublas::sparse_vector_of_sparse_vector<double, ublas::row_major, std::map<std::size_t, std::map<std::size_t, double> > >, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<float>, std::map<std::size_t, std::complex<float> > >,
                          ublas::sparse_vector_of_sparse_vector<std::complex<float>, ublas::row_major, std::map<std::size_t, std::map<std::size_t, std::complex<float> > > >, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>, std::map" << std::endl;
    test_my_matrix_vector<ublas::sparse_vector<std::complex<double>, std::map<std::size_t, std::complex<double> > >,
                          ublas::sparse_vector_of_sparse_vector<std::complex<double>, ublas::row_major, std::map<std::size_t, std::map<std::size_t, std::complex<double> > > >, 3 > () ();
#endif
#endif
#endif
#endif

#ifdef USE_COMPRESSED_MATRIX
#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_my_matrix_vector<ublas::compressed_vector<float>,
                          ublas::compressed_matrix<float>, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_my_matrix_vector<ublas::compressed_vector<double>,
                          ublas::compressed_matrix<double>, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_my_matrix_vector<ublas::compressed_vector<std::complex<float> >,
                          ublas::compressed_matrix<std::complex<float> >, 3 > () ();

    std::cout << "std::complex<double>" << std::endl;
    test_my_matrix_vector<ublas::compressed_vector<std::complex<double> >,
                          ublas::compressed_matrix<std::complex<double> >, 3 > () ();
#endif
#endif
#endif

#ifdef USE_COORDINATE_MATRIX
#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_my_matrix_vector<ublas::coordinate_vector<float>,
                          ublas::coordinate_matrix<float>, 3 > () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_my_matrix_vector<ublas::coordinate_vector<double>,
                          ublas::coordinate_matrix<double>, 3 > () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_my_matrix_vector<ublas::coordinate_vector<std::complex<float> >,
                          ublas::coordinate_matrix<std::complex<float> >, 3 > () ();

    std::cout << "std::complex<double>" << std::endl;
    test_my_matrix_vector<ublas::coordinate_vector<std::complex<double> >,
                          ublas::coordinate_matrix<std::complex<double> >, 3 > () ();
#endif
#endif
#endif
}

