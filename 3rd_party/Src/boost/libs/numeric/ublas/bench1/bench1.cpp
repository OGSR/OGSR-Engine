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
#include <string>

#include <boost/numeric/ublas/config.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <boost/timer.hpp>

#include "bench1.hpp"

void header (std::string text) {
    std::cout << text << std::endl;
    std::cerr << text << std::endl;
}

template<class T>
struct peak_c_plus {
    typedef T value_type;

    void operator () (int runs) const {
        try {
            static T s (0);
            boost::timer t;
            for (int i = 0; i < runs; ++ i) {
                s += T (0);
//                sink_scalar (s);
            }
            footer<value_type> () (0, 1, runs, t.elapsed ());
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
    }
};
template<class T>
struct peak_c_multiplies {
    typedef T value_type;

    void operator () (int runs) const {
        try {
            static T s (1);
            boost::timer t;
            for (int i = 0; i < runs; ++ i) {
                s *= T (1);
//                sink_scalar (s);
            }
            footer<value_type> () (0, 1, runs, t.elapsed ());
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
        }
        catch (...) {
            std::cout << "unknown exception" << std::endl;
        }
    }
};

template<class T>
void peak<T>::operator () (int runs) {
    header ("peak");

    header ("plus");
    peak_c_plus<T> () (runs);

    header ("multiplies");
    peak_c_multiplies<T> () (runs);
}

template struct peak<float>;
template struct peak<double>;
template struct peak<std::complex<float> >;
template struct peak<std::complex<double> >;

#ifdef BOOST_MSVC
// Standard new handler is not standard compliant.
#include <new.h>
int __cdecl std_new_handler (unsigned) {
    throw std::bad_alloc ();
}
#endif

int main (int argc, char *argv []) {
#ifdef BOOST_MSVC
    _set_new_handler (std_new_handler);
#endif

#ifdef USE_FLOAT
    header ("float");
    peak<float> () (100000000);
#endif

#ifdef USE_DOUBLE
    header ("double");
    peak<double> () (100000000);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    header ("std:complex<float>");
    peak<std::complex<float> > () (100000000);
#endif

#ifdef USE_DOUBLE
    header ("std:complex<double>");
    peak<std::complex<double> > () (100000000);
#endif
#endif

    int scale = 1;
    if (argc > 1)
        scale = atoi (argv [1]);

#ifdef USE_FLOAT
    header ("float, 3");

    bench_1<float, 3> () (1000000 * scale);
    bench_2<float, 3> () (300000 * scale);
    bench_3<float, 3> () (100000 * scale);

    header ("float, 10");

    bench_1<float, 10> () (300000 * scale);
    bench_2<float, 10> () (30000 * scale);
    bench_3<float, 10> () (3000 * scale);

    header ("float, 30");

    bench_1<float, 30> () (100000 * scale);
    bench_2<float, 30> () (3000 * scale);
    bench_3<float, 30> () (100 * scale);

    header ("float, 100");

    bench_1<float, 100> () (30000 * scale);
    bench_2<float, 100> () (300 * scale);
    bench_3<float, 100> () (3 * scale);
#endif

#ifdef USE_DOUBLE
    header ("double, 3");

    bench_1<double, 3> () (1000000 * scale);
    bench_2<double, 3> () (300000 * scale);
    bench_3<double, 3> () (100000 * scale);

    header ("double, 10");

    bench_1<double, 10> () (300000 * scale);
    bench_2<double, 10> () (30000 * scale);
    bench_3<double, 10> () (3000 * scale);

    header ("double, 30");

    bench_1<double, 30> () (100000 * scale);
    bench_2<double, 30> () (3000 * scale);
    bench_3<double, 30> () (100 * scale);

    header ("double, 100");

    bench_1<double, 100> () (30000 * scale);
    bench_2<double, 100> () (300 * scale);
    bench_3<double, 100> () (3 * scale);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    header ("std::complex<float>, 3");

    bench_1<std::complex<float>, 3> () (1000000 * scale);
    bench_2<std::complex<float>, 3> () (300000 * scale);
    bench_3<std::complex<float>, 3> () (100000 * scale);

    header ("std::complex<float>, 10");

    bench_1<std::complex<float>, 10> () (300000 * scale);
    bench_2<std::complex<float>, 10> () (30000 * scale);
    bench_3<std::complex<float>, 10> () (3000 * scale);

    header ("std::complex<float>, 30");

    bench_1<std::complex<float>, 30> () (100000 * scale);
    bench_2<std::complex<float>, 30> () (3000 * scale);
    bench_3<std::complex<float>, 30> () (100 * scale);

    header ("std::complex<float>, 100");

    bench_1<std::complex<float>, 100> () (30000 * scale);
    bench_2<std::complex<float>, 100> () (300 * scale);
    bench_3<std::complex<float>, 100> () (3 * scale);
#endif

#ifdef USE_DOUBLE
    header ("std::complex<double>, 3");

    bench_1<std::complex<double>, 3> () (1000000 * scale);
    bench_2<std::complex<double>, 3> () (300000 * scale);
    bench_3<std::complex<double>, 3> () (100000 * scale);

    header ("std::complex<double>, 10");

    bench_1<std::complex<double>, 10> () (300000 * scale);
    bench_2<std::complex<double>, 10> () (30000 * scale);
    bench_3<std::complex<double>, 10> () (3000 * scale);

    header ("std::complex<double>, 30");

    bench_1<std::complex<double>, 30> () (100000 * scale);
    bench_2<std::complex<double>, 30> () (3000 * scale);
    bench_3<std::complex<double>, 30> () (100 * scale);

    header ("std::complex<double>, 100");

    bench_1<std::complex<double>, 100> () (30000 * scale);
    bench_2<std::complex<double>, 100> () (300 * scale);
    bench_3<std::complex<double>, 100> () (3 * scale);
#endif
#endif

    return 0;
}




