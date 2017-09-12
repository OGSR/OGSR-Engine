//-----------------------------------------------------------------------------
// boost mpl/test/lambda_args.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/lambda.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/static_assert.hpp"

using namespace boost;
using namespace mpl;

class C {};
class incomplete;

typedef int C::* mem_ptr;
typedef int (C::* mem_fun_ptr)();

#define AUX_LAMBDA_TEST(T) \
    {BOOST_STATIC_ASSERT((apply1<lambda< is_same<_,T> >::type, T>::type::value));} \
    {BOOST_STATIC_ASSERT((apply1<lambda< is_same<T,_> >::type, T>::type::value));} \
    {BOOST_STATIC_ASSERT((apply2<lambda< is_same<_,_> >::type, T, T>::type::value));} \
/**/

int main()
{
    AUX_LAMBDA_TEST(C);
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1300
    AUX_LAMBDA_TEST(incomplete);
#endif
    AUX_LAMBDA_TEST(int);
    AUX_LAMBDA_TEST(void);
    AUX_LAMBDA_TEST(double);
    AUX_LAMBDA_TEST(int&);
    AUX_LAMBDA_TEST(int*);
//    AUX_LAMBDA_TEST(int[]);
    AUX_LAMBDA_TEST(int[10]);
    AUX_LAMBDA_TEST(int (*)())
    AUX_LAMBDA_TEST(mem_ptr);
    AUX_LAMBDA_TEST(mem_fun_ptr);

    return 0;
}
