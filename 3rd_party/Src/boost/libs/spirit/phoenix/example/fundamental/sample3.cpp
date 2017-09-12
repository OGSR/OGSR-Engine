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
#include "boost/spirit/phoenix/functions.hpp"
#include "boost/spirit/phoenix/primitives.hpp"

using namespace std;
using namespace phoenix;

struct is_odd_ {

    template <typename ArgT>
    struct result { typedef bool type; };

    template <typename ArgT>
    bool operator()(ArgT arg1) const
    { return arg1 % 2 == 1; }
};

function<is_odd_> is_odd;

int
main()
{
    int init[] = { 2, 10, 4, 5, 1, 6, 8, 3, 9, 7 };
    vector<int> c(init, init + 10);
    typedef vector<int>::iterator iterator;

    //  Find the first odd number in container c
    iterator it = find_if(c.begin(), c.end(), is_odd(arg1));

    if (it != c.end())
        cout << *it;    //  if found, print the result
    return 0;
}
