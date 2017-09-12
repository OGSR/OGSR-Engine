/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  Demonstrate regular expression parser objects
//
//  This sample requires an installed version of the boost regex library
//  (http://www.boost.org) The sample was tested with boost V1.28.0
//
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>

#include <boost/spirit/core.hpp>

///////////////////////////////////////////////////////////////////////////////
//
//  The following header must be included, if regular expression support is
//  required for Spirit.
//
//  The BOOST_SPIRIT_NO_REGEX_LIB PP constant should be defined, if you're
//  using the Boost.Regex library from one translation unit only. Otherwise
//  you have to link with the Boost.Regex library as defined in the related
//  documentation (see. http://www.boost.org).
//
///////////////////////////////////////////////////////////////////////////////
#define BOOST_SPIRIT_NO_REGEX_LIB
#include <boost/spirit/utility/regex.hpp>

///////////////////////////////////////////////////////////////////////////////
//  used namespaces
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
// main entry point
int main (int argc, char *argv[])
{
    const char *ptest = "123 E 456";
    const char *prx = "[1-9]+[[:space:]]*E[[:space:]]*";

    cout << "Parse " << ptest << " against regular expression: " << prx
        << endl;

    // 1. direct use of rxlit<>
    rxstrlit<> regexpr(prx);
    parse_info<> result;
    string str;

    result = parse (ptest, regexpr[assign(str)]);
    if (result.hit)
    {
        cout << "Parsed regular expression successfully!" << endl;
        cout << "Matched (" << (int)result.length << ") characters: ";
        cout << "\"" << str << "\"" << endl;
    }
    else
    {
        cout << "Failed to parse regular expression!" << endl;
    }
    cout << endl;

    // 2. use of regex_p predefined parser object
    result = parse (ptest, regex_p(prx)[assign(str)]);
    if (result.hit)
    {
        cout << "Parsed regular expression successfully!" << endl;
        cout << "Matched (" << (int)result.length << ") characters: ";
        cout << "\"" << str << "\"" << endl;
    }
    else
    {
        cout << "Failed to parse regular expression!" << endl;
    }
    cout << endl;

    return 0;
}



