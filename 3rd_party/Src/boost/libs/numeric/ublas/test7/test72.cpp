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

#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/io.hpp>

#include <boost/numeric/ublas/config.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "test7.hpp"

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
            V v1 (N), v2 (N);
            M m1 (N, N);
            (*this) (v1, v2, m1);

            ublas::matrix_row<M> mr1 (m1, 0), mr2 (m1, 1);
            (*this) (mr1, mr2, m1);

            ublas::matrix_column<M> mc1 (m1, 0), mc2 (m1, 1);
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

#ifdef USE_MATRIX
#ifdef USE_BOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, ublas::bounded_array<boost::numeric::interval<float>, 3> >,
                          ublas::matrix<boost::numeric::interval<float>, ublas::row_major, ublas::bounded_array<boost::numeric::interval<float>, 3 * 3> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, ublas::bounded_array<boost::numeric::interval<double>, 3> >,
                          ublas::matrix<boost::numeric::interval<double>, ublas::row_major, ublas::bounded_array<boost::numeric::interval<double>, 3 * 3> >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, ublas::bounded_array<boost::complex<boost::numeric::interval<float> >, 3> >,
                          ublas::matrix<boost::complex<boost::numeric::interval<float> >, ublas::row_major, ublas::bounded_array<boost::complex<boost::numeric::interval<float> >, 3 * 3> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, ublas::bounded_array<boost::complex<boost::numeric::interval<double> >, 3> >,
                          ublas::matrix<boost::complex<boost::numeric::interval<double> >, ublas::row_major, ublas::bounded_array<boost::complex<boost::numeric::interval<double> >, 3 * 3> >, 3> () ();
#endif
#endif
#endif

#ifdef USE_UNBOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, ublas::unbounded_array<boost::numeric::interval<float> > >,
                          ublas::matrix<boost::numeric::interval<float>, ublas::row_major, ublas::unbounded_array<boost::numeric::interval<float> > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, ublas::unbounded_array<boost::numeric::interval<double> > >,
                          ublas::matrix<boost::numeric::interval<double>, ublas::row_major, ublas::unbounded_array<boost::numeric::interval<double> > >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, ublas::unbounded_array<boost::complex<boost::numeric::interval<float> > > >,
                          ublas::matrix<boost::complex<boost::numeric::interval<float> >, ublas::row_major, ublas::unbounded_array<boost::complex<boost::numeric::interval<float> > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, ublas::unbounded_array<boost::complex<boost::numeric::interval<double> > > >,
                          ublas::matrix<boost::complex<boost::numeric::interval<double> >, ublas::row_major, ublas::unbounded_array<boost::complex<boost::numeric::interval<double> > > >, 3> () ();
#endif
#endif
#endif

#ifdef USE_STD_VECTOR
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, std::vector<boost::numeric::interval<float> > >,
                          ublas::matrix<boost::numeric::interval<float>, ublas::row_major, std::vector<boost::numeric::interval<float> > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, std::vector<boost::numeric::interval<double> > >,
                          ublas::matrix<boost::numeric::interval<double>, ublas::row_major, std::vector<boost::numeric::interval<double> > >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, std::vector<boost::complex<boost::numeric::interval<float> > > >,
                          ublas::matrix<boost::complex<boost::numeric::interval<float> >, ublas::row_major, std::vector<boost::complex<boost::numeric::interval<float> > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, std::vector<boost::complex<boost::numeric::interval<double> > > >,
                          ublas::matrix<boost::complex<boost::numeric::interval<double> >, ublas::row_major, std::vector<boost::complex<boost::numeric::interval<double> > > >, 3> () ();
#endif
#endif
#endif
#endif

#ifdef USE_VECTOR_OF_VECTOR
#ifdef USE_BOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, ublas::bounded_array<boost::numeric::interval<float>, 3> >,
                          ublas::vector_of_vector<boost::numeric::interval<float>, ublas::row_major, ublas::bounded_array<ublas::bounded_array<boost::numeric::interval<float>, 3>, 3 + 1> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, ublas::bounded_array<boost::numeric::interval<double>, 3> >,
                          ublas::vector_of_vector<boost::numeric::interval<double>, ublas::row_major, ublas::bounded_array<ublas::bounded_array<boost::numeric::interval<double>, 3>, 3 + 1> >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, ublas::bounded_array<boost::complex<boost::numeric::interval<float> >, 3> >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<float> >, ublas::row_major, ublas::bounded_array<ublas::bounded_array<boost::complex<boost::numeric::interval<float> >, 3>, 3 + 1> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, bounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, ublas::bounded_array<boost::complex<boost::numeric::interval<double> >, 3> >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<double> >, ublas::row_major, ublas::bounded_array<ublas::bounded_array<boost::complex<boost::numeric::interval<double> >, 3>, 3 + 1> >, 3> () ();
#endif
#endif
#endif

#ifdef USE_UNBOUNDED_ARRAY
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, ublas::unbounded_array<boost::numeric::interval<float> > >,
                          ublas::vector_of_vector<boost::numeric::interval<float>, ublas::row_major, ublas::unbounded_array<ublas::unbounded_array<boost::numeric::interval<float> > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, ublas::unbounded_array<boost::numeric::interval<double> > >,
                          ublas::vector_of_vector<boost::numeric::interval<double>, ublas::row_major, ublas::unbounded_array<ublas::unbounded_array<boost::numeric::interval<double> > > >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, ublas::unbounded_array<boost::complex<boost::numeric::interval<float> > > >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<float> >, ublas::row_major, ublas::unbounded_array<ublas::unbounded_array<boost::complex<boost::numeric::interval<float> > > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, unbounded_array" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, ublas::unbounded_array<boost::complex<boost::numeric::interval<double> > > >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<double> >, ublas::row_major, ublas::unbounded_array<ublas::unbounded_array<boost::complex<boost::numeric::interval<double> > > > >, 3> () ();
#endif
#endif
#endif

#ifdef USE_STD_VECTOR
#ifdef USE_FLOAT
    std::cout << "boost::numeric::interval<float>, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<float>, std::vector<boost::numeric::interval<float> > >,
                          ublas::vector_of_vector<boost::numeric::interval<float>, ublas::row_major, std::vector<std::vector<boost::numeric::interval<float> > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::numeric::interval<double>, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::numeric::interval<double>, std::vector<boost::numeric::interval<double> > >,
                          ublas::vector_of_vector<boost::numeric::interval<double>, ublas::row_major, std::vector<std::vector<boost::numeric::interval<double> > > >, 3> () ();
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    std::cout << "boost::complex<boost::numeric::interval<float> >, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<float> >, std::vector<boost::complex<boost::numeric::interval<float> > > >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<float> >, ublas::row_major, std::vector<std::vector<boost::complex<boost::numeric::interval<float> > > > >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "boost::complex<boost::numeric::interval<double> >, std::vector" << std::endl;
    test_my_matrix_vector<ublas::vector<boost::complex<boost::numeric::interval<double> >, std::vector<boost::complex<boost::numeric::interval<double> > > >,
                          ublas::vector_of_vector<boost::complex<boost::numeric::interval<double> >, ublas::row_major, std::vector<std::vector<boost::complex<boost::numeric::interval<double> > > > >, 3> () ();
#endif
#endif
#endif
#endif
}


