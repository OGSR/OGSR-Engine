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
//  A Roman Numerals Parser (demonstrating the symbol table)
//
//  [ JDG 8/22/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Parse roman hundreds (100..900) numerals using the symbol table.
//  Notice that the data associated with each slot is passed
//  to attached semantic actions.
//
///////////////////////////////////////////////////////////////////////////////
struct hundreds : symbols<unsigned>
{
    hundreds()
    {
        add
            ("C"    , 100)
            ("CC"   , 200)
            ("CCC"  , 300)
            ("CD"   , 400)
            ("D"    , 500)
            ("DC"   , 600)
            ("DCC"  , 700)
            ("DCCC" , 800)
            ("CM"   , 900)
        ;
    }

} hundreds_p;

///////////////////////////////////////////////////////////////////////////////
//
//  Parse roman tens (10..90) numerals using the symbol table.
//
///////////////////////////////////////////////////////////////////////////////
struct tens : symbols<unsigned>
{
    tens()
    {
        add
            ("X"    , 10)
            ("XX"   , 20)
            ("XXX"  , 30)
            ("XL"   , 40)
            ("L"    , 50)
            ("LX"   , 60)
            ("LXX"  , 70)
            ("LXXX" , 80)
            ("XC"   , 90)
        ;
    }

} tens_p;

///////////////////////////////////////////////////////////////////////////////
//
//  Parse roman ones (1..9) numerals using the symbol table.
//
///////////////////////////////////////////////////////////////////////////////
struct ones : symbols<unsigned>
{
    ones()
    {
        add
            ("I"    , 1)
            ("II"   , 2)
            ("III"  , 3)
            ("IV"   , 4)
            ("V"    , 5)
            ("VI"   , 6)
            ("VII"  , 7)
            ("VIII" , 8)
            ("IX"   , 9)
        ;
    }

} ones_p;

///////////////////////////////////////////////////////////////////////////////
//
//  Semantic actions
//
///////////////////////////////////////////////////////////////////////////////
struct add_1000
{
    add_1000(unsigned& r_) : r(r_) {}
    void operator()(char) const { r += 1000; }
    unsigned& r;
};

struct add_roman
{
    add_roman(unsigned& r_) : r(r_) {}
    void operator()(unsigned n) const { r += n; }
    unsigned& r;
};

///////////////////////////////////////////////////////////////////////////////
//
//  roman (numerals) grammar
//
///////////////////////////////////////////////////////////////////////////////
struct roman : public grammar<roman>
{
    template <typename ScannerT>
    struct definition
    {
        definition(roman const& self)
        {
            first
                =   +ch_p('M')  [add_1000(self.r)]
                ||  hundreds_p  [add_roman(self.r)]
                ||  tens_p      [add_roman(self.r)]
                ||  ones_p      [add_roman(self.r)];

            //  Note the use of the || operator. The expression
            //  a || b reads match a or b and in sequence. Try
            //  defining the roman numerals grammar in YACC or
            //  PCCTS. Spirit rules! :-)
        }

        rule<ScannerT> first;
        rule<ScannerT> const&
        start() const { return first; }
    };

    roman(unsigned& r_) : r(r_) {}
    unsigned& r;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Main driver code
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tRoman Numerals Parser\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "Type a Roman Numeral ...or [q or Q] to quit\n\n";

    //  Start grammar definition

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        unsigned n = 0;
        roman roman_p(n);
        if (parse(str.c_str(), roman_p).full)
        {
            cout << "parsing succeeded\n";
            cout << "result = " << n << "\n\n";
        }
        else
        {
            cout << "parsing failed\n\n";
        }
    }

    cout << "Bye... :-) \n\n";

    return 0;
}
