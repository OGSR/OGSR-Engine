/*=============================================================================
    Phoenix V1.0
    Copyright (c) 2001-2003 Joel de Guzman

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
==============================================================================*/
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>

#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/composite.hpp"
#include "boost/spirit/phoenix/functions.hpp"
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/binders.hpp"
#include "boost/spirit/phoenix/special_ops.hpp"

using namespace phoenix;
using namespace std;

    ///////////////////////////////////////////////////////////////////////////////
    struct print_ { // a typical STL style monomorphic functor

        typedef void result_type;
        void operator()(int n0)         { cout << "got 1 arg " << n0 << " \n"; }
    };

    functor<print_> print = print_();

///////////////////////////////////////////////////////////////////////////////
int
main()
{

///////////////////////////////////////////////////////////////////////////////
//
//  STL algorithms
//
///////////////////////////////////////////////////////////////////////////////
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);

    for_each(v.begin(), v.end(), arg1 *= 2);
    for (int m = 0; m < 5; ++m, (cout << ','))
    {
        cout << v[m];
        assert(v[m] == (m+1)*2);
    }
    cout << endl;

    for_each(v.begin(), v.end(), print(arg1));

    vector<int>::iterator it = find_if(v.begin(), v.end(), arg1 > 5);
    if (it != v.end())
        cout << *it << endl;

///////////////////////////////////////////////////////////////////////////////
//
//  STL iterators and containers
//
///////////////////////////////////////////////////////////////////////////////

    assert((arg1[0])(v) == v[0]);
    assert((arg1[1])(v) == v[1]);

    list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);
    l.push_back(5);

    list<int>::iterator first = l.begin();
    list<int>::iterator last = l.end();

    assert((*(++arg1))(first) == 2);
    assert((*(----arg1))(last) == 4);

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
