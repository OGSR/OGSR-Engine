/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#ifdef __GNUC__   //  Darn these relops!
#ifndef __SGI_STL_INTERNAL_RELOPS
#define __SGI_STL_INTERNAL_RELOPS
#endif
#endif

#include <iostream>
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
    int i2 = 2, i3 = 3, i = 5;
    const char* world = " world";

///////////////////////////////////////////////////////////////////////////////
//
//  Binary operators
//
///////////////////////////////////////////////////////////////////////////////
    assert((var(i) = var(i))() == 5);
    assert((var(i) = 3)() == 3);
    assert(i == 3);
    i = 5;
    int x, y, z;
    (var(x) = var(y) = var(z) = 10)();
    assert(x == 10 && y == 10 && z == 10);
    assert((val(world)[3])() == world[3]);

    assert((var(i) += 5)() == 10);
    assert((var(i) -= 5)() == 5);
    assert((var(i) *= 5)() == 25);
    assert((var(i) /= 5)() == 5);
    assert((var(i) %= 2)() == 1);

    assert((var(i) <<= 3)() == 8);
    assert((var(i) >>= 1)() == 4);
    assert((var(i) |= 0xFF)() == 0xFF);
    assert((var(i) &= 0xF0)() == 0xF0);
    assert((var(i) ^= 0xFFFFFFFF)() == int(0xFFFFFF0F));

    assert((val(5) == val(5))());
    assert((val(5) == 5)());

    assert((arg1 + arg2)(i2, i3) == i2 + i3);
    assert((arg1 - arg2)(i2, i3) == i2 - i3);
    assert((arg1 * arg2)(i2, i3) == i2 * i3);
    assert((arg1 / arg2)(i2, i3) == i2 / i3);
    assert((arg1 % arg2)(i2, i3) == i2 % i3);
    assert((arg1 & arg2)(i2, i3) == i2 & i3);
    assert((arg1 | arg2)(i2, i3) == i2 | i3);
    assert((arg1 ^ arg2)(i2, i3) == i2 ^ i3);
    assert((arg1 << arg2)(i2, i3) == i2 << i3);
    assert((arg1 >> arg2)(i2, i3) == i2 >> i3);

    assert((val(5) != val(6))());
    assert((val(5) < val(6))());
    assert(!(val(5) > val(6))());
    assert((val(5) < val(6))());
    assert((val(5) <= val(6))());
    assert((val(5) <= val(5))());
    assert((val(7) >= val(6))());
    assert((val(7) >= val(7))());

    assert((val(false) && val(false))() == false);
    assert((val(true) && val(false))() == false);
    assert((val(false) && val(true))() == false);
    assert((val(true) && val(true))() == true);

    assert((val(false) || val(false))() == false);
    assert((val(true) || val(false))() == true);
    assert((val(false) || val(true))() == true);
    assert((val(true) || val(true))() == true);

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
