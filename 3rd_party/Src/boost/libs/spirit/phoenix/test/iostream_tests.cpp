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
#include <algorithm>
#include <string>
#include <cassert>

#include "boost/config.hpp"
#ifdef BOOST_NO_STRINGSTREAM
#include <strstream>
#define SSTREAM strstream
std::string GETSTRING(std::strstream& ss)
{
    ss << ends;
    std::string rval = ss.str();
    ss.freeze(false);
    return rval;
}
#else
#include <sstream>
#define GETSTRING(ss) ss.str()
#define SSTREAM stringstream
#endif

//#define PHOENIX_LIMIT 15
#include "boost/spirit/phoenix/primitives.hpp"
#include "boost/spirit/phoenix/composite.hpp"
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/special_ops.hpp"

using namespace phoenix;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    int     i100 = 100;
    string hello = "hello";
    const char* world = " world";

///////////////////////////////////////////////////////////////////////////////
//
//  IO streams
//
///////////////////////////////////////////////////////////////////////////////
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);

    char const* msg = "cout assert\n";
    (cout << arg1)(msg);
    (cout << val(hello) << world << ", you da man!\n")();
    for_each(v.begin(), v.end(), cout << arg1 << ',');
    cout << endl;

#ifdef __BORLANDC__ // *** See special_ops.hpp why ***
    (cout << arg1 << "this is it, shukz:" << hex_ << arg2 << endl_ << endl_)(msg, i100);
#else
    (cout << arg1 << "this is it, shukz:" << hex << arg2 << endl << endl)(msg, i100);
#endif
    int in;
    int out = 12345;
    SSTREAM sstr;
    (sstr << arg1)(out);
    (sstr >> arg1)(in);
    assert(in == out);

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
