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

#ifdef BOOST_MSVC
// Standard new handler is not standard compliant.
#include <new.h>
int __cdecl std_new_handler (unsigned) {
    throw std::bad_alloc ();
}
#endif

int main () {
#ifdef BOOST_MSVC
    _set_new_handler (std_new_handler);
#endif

    std::cout << "test_blas_1" << std::endl;

#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_blas_1<ublas::vector<float>, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_blas_1<ublas::vector<double>, 3> () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_blas_1<ublas::vector<std::complex<float> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>" << std::endl;
    test_blas_1<ublas::vector<std::complex<double> >, 3> () ();
#endif
#endif

    std::cout << "test_blas_2" << std::endl;

#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_blas_2<ublas::vector<float>, ublas::matrix<float>, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_blas_2<ublas::vector<double>, ublas::matrix<double>, 3> () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_blas_2<ublas::vector<std::complex<float> >, ublas::matrix<std::complex<float> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>" << std::endl;
    test_blas_2<ublas::vector<std::complex<double> >, ublas::matrix<std::complex<double> >, 3> () ();
#endif
#endif

    std::cout << "test_blas_3" << std::endl;

#ifdef USE_FLOAT
    std::cout << "float" << std::endl;
    test_blas_3<ublas::matrix<float>, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "double" << std::endl;
    test_blas_3<ublas::matrix<double>, 3> () ();
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    std::cout << "std::complex<float>" << std::endl;
    test_blas_3<ublas::matrix<std::complex<float> >, 3> () ();
#endif

#ifdef USE_DOUBLE
    std::cout << "std::complex<double>" << std::endl;
    test_blas_3<ublas::matrix<std::complex<double> >, 3> () ();
#endif
#endif

    return 0;
}

