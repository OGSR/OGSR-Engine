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

#ifndef TEST2_H
#define TEST2_H

namespace ublas = boost::numeric::ublas;

template<class V>
void initialize_vector (V &v) {
    int size = v.size ();
    for (int i = 0; i < size; ++ i)
        v [i] = i + 1.f;
}

template<class M>
void initialize_matrix (M &m, ublas::lower_tag) {
    int size1 = m.size1 ();
    int size2 = m.size2 ();
    for (int i = 0; i < size1; ++ i) {
        int j = 0;
        for (; j <= i; ++ j)
            m (i, j) = i * size1 + j + 1.f;
        for (; j < size2; ++ j)
            m (i, j) = 0.f;
    }
}
template<class M>
void initialize_matrix (M &m, ublas::upper_tag) {
    int size1 = m.size1 ();
    int size2 = m.size2 ();
    for (int i = 0; i < size1; ++ i) {
        int j = 0;
        for (; j < i; ++ j)
            m (i, j) = 0.f;
        for (; j < size2; ++ j)
            m (i, j) = i * size1 + j + 1.f;
    }
}
template<class M>
void initialize_matrix (M &m) {
    int size1 = m.size1 ();
    int size2 = m.size2 ();
    for (int i = 0; i < size1; ++ i)
        for (int j = 0; j < size2; ++ j)
            m (i, j) = i * size1 + j + 1.f;
}

template<class V, int N>
struct test_blas_1 {
    typedef typename V::value_type value_type;
    typedef typename ublas::type_traits<value_type>::real_type real_type;

    void operator () ();
};

template<class V, class M, int N>
struct test_blas_2 {
    typedef typename V::value_type value_type;

    void operator () ();
};

template<class M, int N>
struct test_blas_3 {
    typedef typename M::value_type value_type;

    void operator () ();
};

#define USE_FLOAT
#define USE_DOUBLE
#ifndef BOOST_MSVC
#define USE_STD_COMPLEX
#endif

#endif


