/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 1998-2003 Joel de Guzman
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <iostream>
#include <cassert>

using namespace std;

#include "boost/spirit/core.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Primitives tests
//
///////////////////////////////////////////////////////////////////////////////
void
primitives_tests()
{
    char const* cp = "xyz.Jambalaya";
    char const* cp_first = cp;
    char const* cp_last = cp + strlen(cp);

    chlit<> cpx('x');
    parse_info<char const*> pi = parse(cp_first, cp_last, cpx);
    assert(pi.hit);
    assert(!pi.full);
    assert(pi.length == 1);
    assert(pi.stop == &cp[1]);

    pi = parse(pi.stop, ch_p('y'));
    assert(pi.hit);
    assert(!pi.full);
    assert(pi.length == 1);
    assert(pi.stop == &cp[2]);

    scanner<char const*> scan(pi.stop, cp_last);
    match<char> hit = ch_p('z').parse(scan);
    assert(hit);
    assert(hit.value() == 'z');
    assert(pi.stop == &cp[3]);

    pi = parse(pi.stop, eps_p);
    assert(pi.hit);
    assert(!pi.full);
    assert(pi.length == 0);
    assert(pi.stop == &cp[3]);

    pi = parse(pi.stop, nothing_p);
    assert(!pi.hit);
    assert(!pi.full);
    assert(pi.stop == &cp[3]);

    pi = parse(pi.stop, anychar_p);
    assert(pi.hit);
    assert(!pi.full);
    assert(pi.length == 1);
    assert(pi.stop == &cp[4]);

    scan.first = pi.stop;
    hit = range_p('A','Z').parse(scan);
    assert(hit);
    assert(hit.value() == 'J');
    assert(pi.stop == &cp[5]);

    strlit<char const*>     sp("ambalaya");
    strlit<wchar_t const*>  wsp(L"ambalaya");

    char const* save = pi.stop;
    pi = parse(save, sp);
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 8);
    assert(pi.stop == cp_last);

    pi = parse(save, wsp);
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 8);
    assert(pi.stop == cp_last);

    pi = parse("\n", eol_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("\r", eol_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("\r\n", eol_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("\n\r", eol_p);
    assert(pi.hit);
    assert(!pi.full);

    pi = parse("", end_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("1", ~alpha_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("a", ~alpha_p);
    assert(!pi.hit);

    pi = parse("a", ~~alpha_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("1", ~~alpha_p);
    assert(!pi.hit);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    primitives_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

