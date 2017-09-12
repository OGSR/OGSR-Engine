/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#include <iostream>
#include <cmath>
#include <cassert>

#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/composite.hpp"
#include "boost/spirit/phoenix/functions.hpp"
#include "boost/spirit/phoenix/operators.hpp"

using namespace phoenix;
using namespace std;

    ///////////////////////////////////////////////////////////////////////////////
    struct test_ {

        typedef void result_type;
        void operator()() const { cout << "TEST LAZY FUNCTION\n"; }
    };

    function<test_> test;

    ///////////////////////////////////////////////////////////////////////////////
    struct sqr_ {

        template <typename ArgT>
        struct result { typedef ArgT type; };

        template <typename ArgT>
        ArgT operator()(ArgT n) const { return n * n; }
    };

    function<sqr_> sqr;

    ///////////////////////////////////////////////////////////////////////////////
    struct fact_ {

        template <typename ArgT>
        struct result { typedef ArgT type; };

        template <typename ArgT>
        ArgT operator()(ArgT n) const
        { return (n <= 0) ? 1 : n * this->operator()(n-1); }
    };

    function<fact_> fact;

    ///////////////////////////////////////////////////////////////////////////////
    struct pow_ {

        template <typename Arg1T, typename Arg2T>
        struct result { typedef Arg1T type; };

        template <typename Arg1T, typename Arg2T>
        Arg1T operator()(Arg1T a, Arg2T b) const { return pow(a, b); }
    };

    function<pow_> power;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    int     i5 = 5;
    double  d5 = 5, d3 = 3;

///////////////////////////////////////////////////////////////////////////////
//
//  Lazy functors
//
///////////////////////////////////////////////////////////////////////////////

    test()();
    assert(sqr(arg1)(i5) == (i5*i5));
    assert(fact(4)() == 24);
    assert(fact(arg1)(i5) == 120);
    assert(power(arg1, arg2)(d5, d3) == pow(d5, d3));
    assert((sqr(arg1) + 5)(i5) == ((i5*i5)+5));

///////////////////////////////////////////////////////////////////////////////
//
//  End asserts
//
///////////////////////////////////////////////////////////////////////////////

    cout << "///////////////////////////////////////////////////////////////////////////////\n";
    cout << "\t\tTests concluded\n";
    cout << "\t\tSUCCESS!!!\n";
    cout << "///////////////////////////////////////////////////////////////////////////////\n";
}
