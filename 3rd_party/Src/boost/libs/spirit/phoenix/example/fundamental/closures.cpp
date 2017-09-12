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
#include <cassert>

#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/composite.hpp"
#include "boost/spirit/phoenix/special_ops.hpp"
#include "boost/spirit/phoenix/statements.hpp"
#include "boost/spirit/phoenix/functions.hpp"
#include "boost/spirit/phoenix/closures.hpp"

//////////////////////////////////
using namespace std;
using namespace phoenix;

//////////////////////////////////
int
main()
{
    struct my_closure : closure<int, string, double> {

        member1 num;
        member2 message;
        member3 real;
    };

    my_closure clos;

    {   //  First stack frame
        closure_frame<my_closure::self_t> frame(clos);
        (clos.num = 123)();
        (clos.num += 456)();
        (clos.real = clos.num / 56.5)();
        (clos.message = "Hello " + string("World "))();

        {   //  Second stack frame
            closure_frame<my_closure::self_t> frame(clos);
            (clos.num = 987)();
            (clos.message = "Abracadabra ")();
            (clos.real = clos.num * 1e30)();

            {   //  Third stack frame
                tuple<int, char const*, double> init(-1, "Direct Init ", 3.14);
                closure_frame<my_closure::self_t> frame(clos, init);

                (cout << clos.message << clos.num << ", " << clos.real << '\n')();
            }

            (cout << clos.message << clos.num << ", " << clos.real << '\n')();
        }

        (cout << clos.message << clos.num << ", " << clos.real << '\n')();
    }

    return 0;
}
