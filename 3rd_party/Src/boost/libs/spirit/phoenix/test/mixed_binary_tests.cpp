/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#include <iostream>
#include <string>
#include <cassert>

#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/operators.hpp"

using namespace phoenix;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    int     i1 = 1, i2 = 2, i50 = 50, i100 = 100;
    double  d2_5 = 2.5;
    string hello = "hello";
    const char* world = " world";

///////////////////////////////////////////////////////////////////////////////
//
//  Mixed type operators
//
///////////////////////////////////////////////////////////////////////////////
    assert((arg1 + arg2)(i100, i50) == (i100 + i50));
    assert((arg1 + 3)(i100) == (3 + i100));
    assert((arg1 + arg2)(hello, world) == "hello world");
    assert((arg1 + arg2)(i1, d2_5) == (i1 + d2_5));

    assert((*(arg1 + arg2))(world, i2) == *(world + i2));
    assert((*(arg1 + arg2))(i2, world) == *(i2 + world));
    assert((*(val(world+i2) - arg1))(i2) == *world);

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
