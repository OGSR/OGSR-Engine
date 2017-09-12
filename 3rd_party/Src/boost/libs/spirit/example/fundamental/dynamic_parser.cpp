/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002 Juan Carlos Arevalo-Baeza
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A parser based on dynamic parser statements that parses a decimal integer
//
//  [ JCAB 7/26/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/dynamic/if.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/functor_parser.hpp>
#include <iostream>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace local
{
    template <typename T>
    struct var_wrapper
        : public ::boost::reference_wrapper<T>
    {
        typedef  ::boost::reference_wrapper<T> parent;

        explicit inline var_wrapper(T& t) : parent(t) {}

        inline T& operator()() const { return parent::get(); }
    };

    template <typename T>
    inline var_wrapper<T>
    var(T& t)
    {
        return var_wrapper<T>(t);
    }
} // namespace local
///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Our parser functor
//
///////////////////////////////////////////////////////////////////////////////
struct number_parser {
    typedef int result_t;
    template <typename ScannerT>
    int
    operator()(ScannerT const& scan, result_t& result) const {
        if (scan.at_end()) {
            return -1;
        }
        char ch = *scan;
        if (ch < '0' || ch > '9') {
            return -1;
        }
        result = 0;
        int len = 0;
        do {
            result = result*10 + int(ch - '0');
            ++len;
            ++scan;
        } while (!scan.at_end() && (ch = *scan, ch >= '0' && ch <= '9'));
        return len;
    }
};

functor_parser<number_parser> number_parser_p;

///////////////////////////////////////////////////////////////////////////////
//
//  Our number parser function
//
///////////////////////////////////////////////////////////////////////////////
bool
parse_numbers(char const* str, std::vector<int>& n, bool many)
{
    return parse(str,
                lexeme_d[number_parser_p[append(n)]] >>

                /////////////////////////
                // functors as condition
                if_p(local::var(many)) [
                //if_p (phoenix::var(many)) [
                //if_p (boost::lambda::var(many)) [

                /////////////////////////
                // parser as condition
                //if_p (boost::spirit::ch_p(',')) [

                    *(',' >> lexeme_d[number_parser_p[append(n)]])
                ] .else_p [
                    epsilon_p
                ],
                space_p
            ).full;
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
    cout << "\t\tA number parser implemented as a functor for Spirit...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    cout << "Give me an integer number command\n";
    cout << "Commands:\n";
    cout << "  A <num> --> parses a single number\n";
    cout << "  B <num>, <num>, ... --> parses a series of numbers "
            "separated by commas\n";
    cout << "  Q --> quit\n\n";

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q') {
            break;
        } else if (str[0] == 'a' || str[0] == 'A') {
            std::vector<int> n;
            if (parse_numbers(str.c_str()+1, n, false))
            {
                cout << "-------------------------\n";
                cout << "Parsing succeeded\n";
                cout << str << " Parses OK: " << n[0] << endl;
                cout << "-------------------------\n";
            }
            else
            {
                cout << "-------------------------\n";
                cout << "Parsing failed\n";
                cout << "-------------------------\n";
            }
        } else if (str[0] == 'b' || str[0] == 'B') {
            std::vector<int> n;
            if (parse_numbers(str.c_str()+1, n, true))
            {
                cout << "-------------------------\n";
                cout << "Parsing succeeded\n";
                int size = n.size();
                cout << str << " Parses OK: " << size
                     << " number(s): " << n[0];
                for (int i = 1; i < size; ++i) {
                    cout << ", " << n[i];
                }
                cout << endl;
                cout << "-------------------------\n";
            }
            else
            {
                cout << "-------------------------\n";
                cout << "Parsing failed\n";
                cout << "-------------------------\n";
            }
        } else {
            cout << "-------------------------\n";
            cout << "Unrecognized command!!";
            cout << "-------------------------\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}
