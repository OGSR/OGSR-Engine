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
#include <cassert>

using namespace std;

//#define BOOST_SPIRIT_DEBUG
#include "boost/spirit/core.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Operators tests
//
///////////////////////////////////////////////////////////////////////////////
void
operators_tests()
{
    parse_info<char const*> pi;

    pi = parse("Hello World", str_p("Hello ") >> "World");
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 11);
    assert(*pi.stop == 0);

    pi = parse("Banana", str_p("Banana") | "Pineapple");
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 6);
    assert(*pi.stop == 0);

    pi = parse("Pineapple", str_p("Banana") | "Pineapple");
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 9);
    assert(*pi.stop == 0);

    pi = parse("a.2  ", alpha_p || ('.' >> digit_p));
    assert(pi.hit);
    assert(pi.length == 3);

    pi = parse("a  ", alpha_p || ('.' >> digit_p));
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse(".1  ", alpha_p || ('.' >> digit_p));
    assert(pi.hit);
    assert(pi.length == 2);

    pi = parse("1.a  ", alpha_p || ('.' >> digit_p));
    assert(!pi.hit);

    pi = parse("abcdefghij", +xdigit_p & +alpha_p);
    assert(pi.hit);
    assert(pi.length == 6);

    pi = parse("abcdefghij", +alpha_p & +xdigit_p);
    assert(!pi.hit);

    pi = parse("abcdefghij", +digit_p & +alpha_p);
    assert(!pi.hit);

    pi = parse("abcdefghij", +alpha_p & +digit_p);
    assert(!pi.hit);

    pi = parse("F", xdigit_p - range_p('5','8'));
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("0", xdigit_p - range_p('5','8'));
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("4", xdigit_p - range_p('5','8'));
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("9", xdigit_p - range_p('5','8'));
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("5", xdigit_p - range_p('5','8'));
    assert(!pi.hit);

    pi = parse("7", xdigit_p - range_p('5','8'));
    assert(!pi.hit);

    pi = parse("x*/", anychar_p - "*/");
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("*/", anychar_p - "*/");
    assert(!pi.hit);

    pi = parse("switcher  ", str_p("switcher") - "switch");
    assert(pi.hit);
    assert(pi.length == 8);

    pi = parse("Z", alpha_p ^ xdigit_p);
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("1", alpha_p ^ xdigit_p);
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("B", alpha_p ^ xdigit_p);
    assert(!pi.hit);

    pi = parse("B", !alpha_p);
    assert(pi.hit);
    assert(pi.length == 1);

    pi = parse("1", !alpha_p);
    assert(pi.hit);
    assert(pi.length == 0);
    assert(*pi.stop == '1');

    pi = parse("1234 5678 1234 5678", *(+digit_p >> *space_p));
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 19);
    assert(*pi.stop == 0);

    pi = parse("abcdefghijklmnopqrstuvwxyz123", *alpha_p);
    assert(pi.hit);
    assert(pi.length == 26);

    pi = parse("1234+5678*1234/5678", +digit_p % (ch_p('+') | '*' | '/'));
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 19);
    assert(*pi.stop == 0);

    pi = parse("1234+", +digit_p % '+');
    assert(pi.hit);
    assert(!pi.full);
    assert(pi.length == 4);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    operators_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

