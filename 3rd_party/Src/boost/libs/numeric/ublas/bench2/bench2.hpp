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

#ifndef BENCH2_H
#define BENCH2_H

namespace ublas = boost::numeric::ublas;

void header (std::string text);

template<class T>
struct footer {
    void operator () (int multiplies, int plus, int runs, double elapsed) {
        std::cout << "elapsed: " << elapsed << " s, "
                  << (multiplies * ublas::type_traits<T>::multiplies_complexity +
                      plus * ublas::type_traits<T>::plus_complexity) * runs /
                     (1024 * 1024 * elapsed) << " Mflops" << std::endl;
        std::cerr << "elapsed: " << elapsed << " s, "
                  << (multiplies * ublas::type_traits<T>::multiplies_complexity +
                      plus * ublas::type_traits<T>::plus_complexity) * runs /
                     (1024 * 1024 * elapsed) << " Mflops" << std::endl;
    }
};

template<class T, int N>
struct c_vector_traits {
    typedef T type [N];
};
template<class T, int N, int M>
struct c_matrix_traits {
    typedef T type [N] [M];
};

template<class T, int N>
struct initialize_c_vector  {
#ifdef BOOST_MSVC
    BOOST_UBLAS_INLINE
    void operator () (typename c_vector_traits<T, N>::type v) {
#else
    void operator () (typename c_vector_traits<T, N>::type &v) {
#endif
        for (int i = 0; i < N; ++ i)
            v [i] = rand () * 1.f;
//            v [i] = 0.f;
        }
};
template<class V>
BOOST_UBLAS_INLINE
void initialize_vector (V &v) {
    int size = v.size ();
    for (int i = 0; i < size; ++ i)
        v [i] = rand () * 1.f;
//        v [i] = 0.f;
}

template<class T, int N, int M>
struct initialize_c_matrix  {
#ifdef BOOST_MSVC
    BOOST_UBLAS_INLINE
    void operator () (typename c_matrix_traits<T, N, M>::type m) {
#else
    void operator () (typename c_matrix_traits<T, N, M>::type &m) {
#endif
        for (int i = 0; i < N; ++ i)
            for (int j = 0; j < M; ++ j)
                m [i] [j] = rand () * 1.f;
//                m [i] [j] = 0.f;
    }
};
template<class M>
BOOST_UBLAS_INLINE
void initialize_matrix (M &m, ublas::row_major_tag) {
    int size1 = m.size1 ();
    int size2 = m.size2 ();
    for (int i = 0; i < size1; ++ i)
        for (int j = 0; j < size2; ++ j)
            m (i, j) = rand () * 1.f;
//            m (i, j) = 0.f;
}
template<class M>
BOOST_UBLAS_INLINE
void initialize_matrix (M &m, ublas::column_major_tag) {
    int size1 = m.size1 ();
    int size2 = m.size2 ();
    for (int j = 0; j < size2; ++ j)
        for (int i = 0; i < size1; ++ i)
            m (i, j) = rand () * 1.f;
//            m (i, j) = 0.f;
}
template<class M>
BOOST_UBLAS_INLINE
void initialize_matrix (M &m) {
    typedef BOOST_UBLAS_TYPENAME M::orientation_category orientation_category;
    initialize_matrix (m, orientation_category ());
}

template<class T>
BOOST_UBLAS_INLINE
void sink_scalar (const T &s) {
    static T g_s = s;
}

template<class T, int N>
struct sink_c_vector {
#ifdef BOOST_MSVC
    BOOST_UBLAS_INLINE
    void operator () (const typename c_vector_traits<T, N>::type v) {
#else
    void operator () (const typename c_vector_traits<T, N>::type &v) {
#endif
        static typename c_vector_traits<T, N>::type g_v;
        for (int i = 0; i < N; ++ i)
            g_v [i] = v [i];
    }
};
template<class V>
BOOST_UBLAS_INLINE
void sink_vector (const V &v) {
    static V g_v (v);
}

template<class T, int N, int M>
struct sink_c_matrix {
#ifdef BOOST_MSVC
    BOOST_UBLAS_INLINE
    void operator () (const typename c_matrix_traits<T, N, M>::type m) {
#else
    void operator () (const typename c_matrix_traits<T, N, M>::type &m) {
#endif
    static typename c_matrix_traits<T, N, M>::type g_m;
    for (int i = 0; i < N; ++ i)
        for (int j = 0; j < M; ++ j)
            g_m [i] [j] = m [i] [j];
    }
};
template<class M>
BOOST_UBLAS_INLINE
void sink_matrix (const M &m) {
    static M g_m (m);
}

template<class T>
struct peak {
    void operator () (int runs);
};

template<class T, int N>
struct bench_1 {
    void operator () (int runs);
};

template<class T, int N>
struct bench_2 {
    void operator () (int runs);
};

template<class T, int N>
struct bench_3 {
    void operator () (int runs);
};

struct safe_tag {};
struct fast_tag {};

// #define USE_FLOAT
#define USE_DOUBLE
// #define USE_STD_COMPLEX

#define USE_MAP_ARRAY
// #define USE_STD_MAP
// #define USE_STD_VALARRAY

#define USE_SPARSE_VECTOR
#define USE_COMPRESSED_VECTOR
#define USE_COORDINATE_VECTOR

#define USE_SPARSE_MATRIX
// #define USE_SPARSE_VECTOR_OF_SPARSE_VECTOR
#define USE_COMPRESSED_MATRIX
#define USE_COORDINATE_MATRIX

#endif


