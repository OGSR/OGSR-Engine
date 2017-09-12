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

#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/io.hpp>

#include <boost/numeric/ublas/config.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <boost/timer.hpp>

#include "bench4.hpp"

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

template struct peak<boost::numeric::interval<float> >;
template struct peak<boost::numeric::interval<double> >;

#ifdef USE_BOOST_COMPLEX

template struct peak<boost::complex<boost::numeric::interval<float> > >;
template struct peak<boost::complex<boost::numeric::interval<double> > >;

#endif

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
    header ("boost::numeric::interval<float>");
    peak<boost::numeric::interval<float> > () (100000000);
#endif

#ifdef USE_DOUBLE
    header ("boost::numeric::interval<double>");
    peak<boost::numeric::interval<double> > () (100000000);
#endif

#ifdef USE_STD_COMPLEX
#ifdef USE_FLOAT
    header ("std:complex<boost::numeric::interval<float> >");
    peak<boost::complex<boost::numeric::interval<float> > > () (100000000);
#endif

#ifdef USE_DOUBLE
    header ("std:complex<boost::numeric::interval<double> >");
    peak<boost::complex<boost::numeric::interval<double> > > () (100000000);
#endif
#endif

    int scale = 1;
    if (argc > 1)
        scale = atoi (argv [1]);

#ifdef USE_FLOAT
    header ("boost::numeric::interval<float>, 3");

    bench_1<boost::numeric::interval<float>, 3> () (1000000 * scale);
    bench_2<boost::numeric::interval<float>, 3> () (300000 * scale);
    bench_3<boost::numeric::interval<float>, 3> () (100000 * scale);

    header ("boost::numeric::interval<float>, 10");

    bench_1<boost::numeric::interval<float>, 10> () (300000 * scale);
    bench_2<boost::numeric::interval<float>, 10> () (30000 * scale);
    bench_3<boost::numeric::interval<float>, 10> () (3000 * scale);

    header ("boost::numeric::interval<float>, 30");

    bench_1<boost::numeric::interval<float>, 30> () (100000 * scale);
    bench_2<boost::numeric::interval<float>, 30> () (3000 * scale);
    bench_3<boost::numeric::interval<float>, 30> () (100 * scale);

    header ("boost::numeric::interval<float>, 100");

    bench_1<boost::numeric::interval<float>, 100> () (30000 * scale);
    bench_2<boost::numeric::interval<float>, 100> () (300 * scale);
    bench_3<boost::numeric::interval<float>, 100> () (3 * scale);
#endif

#ifdef USE_DOUBLE
    header ("boost::numeric::interval<double>, 3");

    bench_1<boost::numeric::interval<double>, 3> () (1000000 * scale);
    bench_2<boost::numeric::interval<double>, 3> () (300000 * scale);
    bench_3<boost::numeric::interval<double>, 3> () (100000 * scale);

    header ("boost::numeric::interval<double>, 10");

    bench_1<boost::numeric::interval<double>, 10> () (300000 * scale);
    bench_2<boost::numeric::interval<double>, 10> () (30000 * scale);
    bench_3<boost::numeric::interval<double>, 10> () (3000 * scale);

    header ("boost::numeric::interval<double>, 30");

    bench_1<boost::numeric::interval<double>, 30> () (100000 * scale);
    bench_2<boost::numeric::interval<double>, 30> () (3000 * scale);
    bench_3<boost::numeric::interval<double>, 30> () (100 * scale);

    header ("boost::numeric::interval<double>, 100");

    bench_1<boost::numeric::interval<double>, 100> () (30000 * scale);
    bench_2<boost::numeric::interval<double>, 100> () (300 * scale);
    bench_3<boost::numeric::interval<double>, 100> () (3 * scale);
#endif

#ifdef USE_BOOST_COMPLEX
#ifdef USE_FLOAT
    header ("boost::complex<boost::numeric::interval<float> >, 3");

    bench_1<boost::complex<boost::numeric::interval<float> >, 3> () (1000000 * scale);
    bench_2<boost::complex<boost::numeric::interval<float> >, 3> () (300000 * scale);
    bench_3<boost::complex<boost::numeric::interval<float> >, 3> () (100000 * scale);

    header ("boost::complex<boost::numeric::interval<float> >, 10");

    bench_1<boost::complex<boost::numeric::interval<float> >, 10> () (300000 * scale);
    bench_2<boost::complex<boost::numeric::interval<float> >, 10> () (30000 * scale);
    bench_3<boost::complex<boost::numeric::interval<float> >, 10> () (3000 * scale);

    header ("boost::complex<boost::numeric::interval<float> >, 30");

    bench_1<boost::complex<boost::numeric::interval<float> >, 30> () (100000 * scale);
    bench_2<boost::complex<boost::numeric::interval<float> >, 30> () (3000 * scale);
    bench_3<boost::complex<boost::numeric::interval<float> >, 30> () (100 * scale);

    header ("boost::complex<boost::numeric::interval<float> >, 100");

    bench_1<boost::complex<boost::numeric::interval<float> >, 100> () (30000 * scale);
    bench_2<boost::complex<boost::numeric::interval<float> >, 100> () (300 * scale);
    bench_3<boost::complex<boost::numeric::interval<float> >, 100> () (3 * scale);
#endif

#ifdef USE_DOUBLE
    header ("boost::complex<boost::numeric::interval<double> >, 3");

    bench_1<boost::complex<boost::numeric::interval<double> >, 3> () (1000000 * scale);
    bench_2<boost::complex<boost::numeric::interval<double> >, 3> () (300000 * scale);
    bench_3<boost::complex<boost::numeric::interval<double> >, 3> () (100000 * scale);

    header ("boost::complex<boost::numeric::interval<double> >, 10");

    bench_1<boost::complex<boost::numeric::interval<double> >, 10> () (300000 * scale);
    bench_2<boost::complex<boost::numeric::interval<double> >, 10> () (30000 * scale);
    bench_3<boost::complex<boost::numeric::interval<double> >, 10> () (3000 * scale);

    header ("boost::complex<boost::numeric::interval<double> >, 30");

    bench_1<boost::complex<boost::numeric::interval<double> >, 30> () (100000 * scale);
    bench_2<boost::complex<boost::numeric::interval<double> >, 30> () (3000 * scale);
    bench_3<boost::complex<boost::numeric::interval<double> >, 30> () (100 * scale);

    header ("boost::complex<boost::numeric::interval<double> >, 100");

    bench_1<boost::complex<boost::numeric::interval<double> >, 100> () (30000 * scale);
    bench_2<boost::complex<boost::numeric::interval<double> >, 100> () (300 * scale);
    bench_3<boost::complex<boost::numeric::interval<double> >, 100> () (3 * scale);
#endif
#endif

    return 0;
}

