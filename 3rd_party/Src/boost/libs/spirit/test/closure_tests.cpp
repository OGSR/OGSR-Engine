/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Joel de Guzman
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
#include "boost/spirit/attribute/closure.hpp"
#include "boost/spirit/attribute/parametric.hpp"
#include "boost/spirit/phoenix/binders.hpp"
using namespace boost::spirit;
using namespace phoenix;

///////////////////////////////////////////////////////////////////////////////
//
//  Closure tests
//
///////////////////////////////////////////////////////////////////////////////
struct my_closure1 : boost::spirit::closure<my_closure1, double>
{
    member1 val;
};

struct my_closure2 : boost::spirit::closure<my_closure2, char>
{
    member1 ch;
};

struct my_closure3 : boost::spirit::closure<my_closure3, char>
{
    member1 ch;
};

struct X { int a; int b; };

struct my_closure4 : boost::spirit::closure<my_closure4, X>
{
    member1 x;
};

struct Y { Y(int) {} };

struct my_closure5 : boost::spirit::closure<my_closure5, int, Y>
{
    member1 y;
};

struct my_closure6 : boost::spirit::closure<my_closure6, int, int, int>
{
    member1 x;
    member2 y;
    member3 z;
};

void
closure_tests()
{
    rule<phrase_scanner_t, my_closure1::context_t> num_list;
    double n;

    num_list =
    (
        real_p[num_list.val = arg1] >>  *(',' >> real_p[num_list.val += arg1])
    )
    [var(n) = num_list.val];

    parse_info<char const*> pi;
    pi = parse("123, 456, 789", num_list, space_p);
    assert(pi.hit);
    assert(pi.full);
    assert(n == 123 + 456 + 789);

    rule<scanner<>, my_closure2::context_t> rev;
    rev = anychar_p[rev.ch = arg1] >> !rev >> f_ch_p(rev.ch);

    pi = parse("xyzzyx", rev);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("xyzczyx", rev);
    assert(!pi.hit);

    subrule<0, my_closure3::context_t> rev2;
    pi = parse("atoyyota",
        rev2 = anychar_p[rev2.ch = arg1] >> !rev2 >> f_ch_p(rev2.ch)
    );

    assert(pi.hit);
    assert(pi.full);

    pi = parse("whatdahell",
        rev2 = anychar_p[rev2.ch = arg1] >> !rev2 >> f_ch_p(rev2.ch)
    );
    assert(!pi.hit);

    rule<phrase_scanner_t, my_closure4::context_t> complex_p;
    complex_p =
            int_p[bind(&X::a)(complex_p.x) = arg1]
        >>  ','
        >>  int_p[bind(&X::b)(complex_p.x) = arg1]
    ;

    X x;
    pi = parse("123, 456", complex_p[var(x) = arg1], space_p);
    assert(pi.hit);
    assert(x.a == 123);
    assert(x.b == 456);

    rule<scanner<>, my_closure5::context_t> init1; // compile check only
    rule<> r1 = init1(3, 3); // member2 is constructed from int

    rule<scanner<>, my_closure6::context_t> init2; // compile check only
    rule<> r2 = init2(3); // member2 and member3 are default constructed
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    closure_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

