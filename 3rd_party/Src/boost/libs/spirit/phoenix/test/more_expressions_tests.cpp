/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#include <iostream>
#include <cassert>

#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/composite.hpp"
#include "boost/spirit/phoenix/functions.hpp"
#include "boost/spirit/phoenix/operators.hpp"

using namespace phoenix;
using namespace std;

    ///////////////////////////////////////////////////////////////////////////////
    struct sqr_ {

        template <typename ArgT>
        struct result { typedef ArgT type; };

        template <typename ArgT>
        ArgT operator()(ArgT n) const { return n * n; }
    };

    function<sqr_> sqr;

    ///////////////////////////////////////////////////////////////////////////////
    struct adder_ {

        template <typename Arg1T, typename Arg2T, typename ArgT3>
        struct result { typedef Arg1T type; };

        template <typename Arg1T, typename Arg2T, typename ArgT3>
        Arg1T operator()(Arg1T a, Arg2T b, ArgT3 c) const { return a + b + c; }
    };

    function<adder_> adder;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    int     i2 = 2, i = 4, i50 = 50, i10 = 10, i20 = 20, i100 = 100;
    double  d5 = 5, d10 = 10;
    string hello = "hello";

///////////////////////////////////////////////////////////////////////////////
//
//  More complex expressions
//
///////////////////////////////////////////////////////////////////////////////
    assert((10 - arg1)(i100) == (10 - i100));
    assert((20 - arg1)(i100) == (20 - i100));
    assert((arg1 - 10)(i100) == (i100 - 10));
    assert((arg1 - 20)(i100) == (i100 - 20));
    assert((arg1 - arg2)(i100, i50) == (i100 - i50));
    assert((arg1 - var(i))(i10) == (i10 - i));
    assert((arg1 + arg2 - arg3)(i100, i50, i20) == (i100 + i50 - i20));
    assert((sqr(arg1) + arg2 - arg3)(i100, i50, i20) == ((i100*i100) + i50 - i20));

    int ii = i;
    assert((var(i) += arg1)(i2) == (ii += i2));
    assert((sqr(sqr(arg1)))(i100) == (i100*i100*i100*i100));


#if 0   /*** SHOULD NOT COMPILE ***/
    (val(3) += arg1)(i100);
    (val(3) = 3)();
#endif

    assert(((adder(arg1, arg2, arg3) + arg2 - arg3)(i100, i50, i20)) == (i100 + i50 + i20) + i50 - i20);
    assert((adder(arg1, arg2, arg3)(i100, i50, i20)) == (i100 + i50 + i20));
    assert((sqr(sqr(sqr(sqr(arg1)))))(d10) == 1e16);
    assert((sqr(sqr(arg1)) / arg1 / arg1)(d5) == 25);

    for (int j = 0; j < 20; ++j)
    {
        cout << (10 < arg1)(j);
        assert((10 < arg1)(j) == (10 < j));
    }
    cout << endl;

    for (int k = 0; k < 20; ++k)
    {
        bool r = ((arg1 % 2 == 0) && (arg1 < 15))(k);
        cout << r;
        assert(r == ((k % 2 == 0) && (k < 15)));
    }
    cout << endl;

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
