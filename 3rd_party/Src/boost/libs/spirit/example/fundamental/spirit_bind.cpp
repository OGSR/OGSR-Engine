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
//  Demonstrates use of boost::bind and spirit
//
//  [ JDG 9/29/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////
//
//  Our comma separated list parser
//
///////////////////////////////////////////////////////////////////////////////
class list_parser
{
public:

    typedef list_parser self_t;

    bool
    parse(char const* str)
    {
        return boost::spirit::parse(str,

            //  Begin grammar
            (
                real_p
                [
                    bind(&self_t::add, this, _1)
                ]

                >> *(   ','
                        >>  real_p
                            [
                                bind(&self_t::add, this, _1)
                            ]
                    )
            )
            ,
            //  End grammar

            space_p).full;
    }

    void
    add(double n)
    {
        v.push_back(n);
    }

    void
    print() const
    {
        for (vector<double>::size_type i = 0; i < v.size(); ++i)
            cout << i << ": " << v[i] << endl;
    }

    vector<double> v;
};

////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\tA comma separated list parser for Spirit...\n";
    cout << "\tDemonstrates use of boost::bind and spirit\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    cout << "Give me a comma separated list of numbers.\n";
    cout << "The numbers will be inserted in a vector of numbers\n";
    cout << "Type [q or Q] to quit\n\n";

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        list_parser lp;
        if (lp.parse(str.c_str()))
        {
            cout << "-------------------------\n";
            cout << "Parsing succeeded\n";
            cout << str << " Parses OK: " << endl;

            lp.print();

            cout << "-------------------------\n";
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


