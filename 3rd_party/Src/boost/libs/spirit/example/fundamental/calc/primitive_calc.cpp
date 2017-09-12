/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A primitive calculator that knows how to add and subtract.
//  [ demonstrating phoenix ]
//
//  [ JDG 6/28/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;
using namespace phoenix;

///////////////////////////////////////////////////////////////////////////////
//
//  Our primitive calculator
//
///////////////////////////////////////////////////////////////////////////////
template <typename IteratorT>
bool primitive_calc(IteratorT first, IteratorT last, double& n)
{
    return parse(first, last,

        //  Begin grammar
        (
            real_p[var(n) = arg1]
            >> *(   ('+' >> real_p[var(n) += arg1])
                |   ('-' >> real_p[var(n) -= arg1])
                )
        )
        ,
        //  End grammar

        space_p).full;
}

////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tA primitive calculator...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    cout << "Give me a list of numbers to be added or subtracted.\n";
    cout << "Example: 1 + 10 + 3 - 4 + 9\n";
    cout << "The result is computed using Phoenix.\n";
    cout << "Type [q or Q] to quit\n\n";

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        double n;
        if (primitive_calc(str.begin(), str.end(), n))
        {
            cout << "-------------------------\n";
            cout << "Parsing succeeded\n";
            cout << str << " Parses OK: " << endl;

            cout << "result = " << n;
            cout << "\n-------------------------\n";
        }
        else
        {
            cout << "-------------------------\n";
            cout << "Parsing failed\n";
            cout << "-------------------------\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}


