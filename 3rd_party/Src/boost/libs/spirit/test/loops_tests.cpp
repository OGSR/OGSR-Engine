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
#include "boost/spirit/utility.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Loops tests
//
///////////////////////////////////////////////////////////////////////////////
void
loops_tests()
{
    parse_info<char const*> pi;

    pi = parse("\"Hello World\"", "\"" >> *(anychar_p - "\"") >> "\"");
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 13);
    assert(*pi.stop == 0);

    pi = parse("\"Hello World\"", "\"" >> repeat_p(0, more)[anychar_p - "\""] >> "\"");
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 13);
    assert(*pi.stop == 0);

    pi = parse("xx", +ch_p('x'));
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 2);
    assert(*pi.stop == 0);

    pi = parse("xx", repeat_p(1, more)[ch_p('x')]);
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 2);
    assert(*pi.stop == 0);

    pi = parse("", +ch_p('x'));
    assert(!pi.hit);

    pi = parse("", repeat_p(1, more)[ch_p('x')]);
    assert(!pi.hit);

    pi = parse("", *ch_p('x'));
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 0);
    assert(*pi.stop == 0);

    pi = parse("", repeat_p(0, more)[ch_p('x')]);
    assert(pi.hit);
    assert(pi.full);
    assert(pi.length == 0);
    assert(*pi.stop == 0);

    //  repeat exact 8
    rule<>  rep8 = repeat_p(8)[alpha_p] >> 'X';
    assert(!parse("abcdefgX", rep8).hit);
    assert(parse("abcdefghX", rep8).full);
    assert(!parse("abcdefghiX", rep8).hit);
    assert(!parse("abcdefgX", rep8).hit);
    assert(!parse("aX", rep8).hit);

    //  repeat 2 to 8
    rule<>  rep28 = repeat_p(2, 8)[alpha_p] >> '*';
    assert(parse("abcdefg*", rep28).full);
    assert(parse("abcdefgh*", rep28).full);
    assert(!parse("abcdefghi*", rep28).hit);
    assert(!parse("a*", rep28).hit);

    //  repeat 2 or more
    rule<>  rep2_ = repeat_p(2, more)[alpha_p] >> '+';
    assert(parse("abcdefg+", rep2_).full);
    assert(parse("abcdefgh+", rep2_).full);
    assert(parse("abcdefghi+", rep2_).full);
    assert(parse("abcdefg+", rep2_).full);
    assert(!parse("a+", rep2_).hit);

    //  repeat 0
    rule<>  rep0 = repeat_p(0)[alpha_p] >> '/';
    assert(parse("/", rep0).full);
    assert(!parse("a/", rep0).hit);

    //  repeat 0 or 1
    rule<>  rep01 = repeat_p(0, 1)[alpha_p >> digit_p] >> '?';
    assert(!parse("abcdefg?", rep01).hit);
    assert(!parse("a?", rep01).hit);
    assert(!parse("1?", rep01).hit);
    assert(!parse("11?", rep01).hit);
    assert(!parse("aa?", rep01).hit);
    assert(parse("?", rep01).full);
    assert(parse("a1?", rep01).full);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    loops_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

