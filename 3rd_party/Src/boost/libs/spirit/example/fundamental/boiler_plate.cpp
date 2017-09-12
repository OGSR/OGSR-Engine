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
//  Boiler plate [ A template for writing your parser ]
//
//  [ JDG 9/17/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Semantic actions
//
///////////////////////////////////////////////////////////////////////////////
struct my_action
{
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        string s(first, last);
        cout << "\tMy Action got: " << s << endl;
    }
};

///////////////////////////////////////////////////////////////////////////////
//
//  My grammar
//
///////////////////////////////////////////////////////////////////////////////
struct my_grammar : public grammar<my_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(my_grammar const& self)
        {
            my_rule =
                *lexeme_d[(+graph_p)[my_action()]]
                ;
        }

        rule<ScannerT> my_rule;
        rule<ScannerT> const&
        start() const { return my_rule; }
    };
};

///////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\t A boiler-plate parser...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "Type anything or [q or Q] to quit\n\n";

    my_grammar g;

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        if (parse(str.c_str(), g, space_p).full)
        {
            cout << "parsing succeeded\n";
        }
        else
        {
            cout << "parsing failed\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}


