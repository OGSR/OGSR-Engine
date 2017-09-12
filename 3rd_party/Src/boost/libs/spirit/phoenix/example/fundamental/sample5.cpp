/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#include <vector>
#include <algorithm>
#include <iostream>
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/primitives.hpp"

using namespace std;
using namespace phoenix;

//////////////////////////////////
template <int N>
struct static_int {

    template <typename TupleT>
    struct result { typedef int type; };

    template <typename TupleT>
    int eval(TupleT const&) const { return N; }
};

//////////////////////////////////
template <int N>
phoenix::actor<static_int<N> >
int_const()
{
    return static_int<N>();
}

//////////////////////////////////
int
main()
{
    cout << (int_const<5>() + int_const<6>())() << endl;
    return 0;
}
