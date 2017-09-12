/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 1998-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <iostream>
#include <string>
#include <cassert>
#include "boost/spirit/core.hpp"
#include "boost/spirit/symbols/symbols.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
void
add(symbols<>& sym, char const* str)
{
    sym.add(str, str + strlen(str));
}

///////////////////////////////////////
void
check(
    symbols<> const& sym,
    char const* str,
    bool hit,
    char const* result,
    int length)
{
    char const* s = str;
    parse_info<>  info = parse(str, sym);

    if (info.hit)
    {
        cout << s << " OK [non-matching=\""
        << str+info.length << "\", matching-length="
        << info.length << "]" << endl;

        assert(hit);
        assert(unsigned(length) == info.length);
        assert(strcmp(str+info.length, result) == 0);
    }
    else
    {
        cout << s << " is not a member" << endl;
        assert(!hit);
        assert(strcmp(str, result) == 0);
    }
}

///////////////////////////////////////
void
action1(int& data)
{
    cout << "storing 123456 into data slot" << endl;
    data = 123456;
}

///////////////////////////////////////
void
action2(int data)
{
    cout << "got: " << data << " from data slot" << endl;
    assert(data == 123456);
}

///////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tSymbol table test\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    symbols<>   sym;
    sym = "pineapple", "orange", "banana", "applepie", "apple";

    cout << "\nsym = \"pineapple\", \"orange\","
    << " \"banana\", \"apple\", \"applepie\";\n\n";

    check(sym, "pineapple", true, "", 9);
    check(sym, "orange", true, "", 6);
    check(sym, "banana", true, "", 6);
    check(sym, "apple", true, "", 5);
    check(sym, "pizza", false, "pizza", -1);
    check(sym, "steak", false, "steak", -1);
    check(sym, "applepie", true, "", 8);
    check(sym, "bananarama", true, "rama", 6);
    check(sym, "applet", true, "t", 5);
    check(sym, "applepi", true, "pi", 5);
    check(sym, "appl", false, "appl", -1);

    symbols<>   sym2 = sym;

    cout << "\nsym2 = sym\n\n";

    check(sym2, "pineapplez", true, "z", 9);
    check(sym2, "orangez", true, "z", 6);
    check(sym2, "bananaz", true, "z", 6);
    check(sym2, "applez", true, "z", 5);
    check(sym2, "pizzaz", false, "pizzaz", -1);
    check(sym2, "steakz", false, "steakz", -1);
    check(sym2, "applepiez", true, "z", 8);
    check(sym2, "bananaramaz", true, "ramaz", 6);
    check(sym2, "appletz", true, "tz", 5);
    check(sym2, "applepix", true, "pix", 5);

    cout << "\nchecking data slot \"orange\" of sym\n";
    parse("orange", sym[&action1]);
    parse("orange", sym[&action2]);
    check(sym, "orange", true, "", 6);

    cout << "\nTest concluded successfully\n";
    return 0;
}
