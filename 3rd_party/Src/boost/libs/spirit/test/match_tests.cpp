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

#include "boost/spirit/core.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Match tests
//
///////////////////////////////////////////////////////////////////////////////
void
match_tests()
{
    match<>         m0;
    match<int>      m1(m0);
    m1.value() = 123;
    match<double>   m2(m1);
    assert(m1.value() == int(m2.value()));
    m2.value() = 456;

    m0 = m0;
    m0 = m1;
    m0 = m2;
    m1 = m0;
    assert(m1.value() == 0);
    m1 = m1;
    m1 = m2;
    assert(m1.value() == int(m2.value()));
    m1.value() = 123;
    m2 = m0;
    assert(m2.value() == 0);
    m2 = m1;
    assert(m1.value() == int(m2.value()));
    m2 = m2;

    cout << "sizeof(int) == " << sizeof(int) << '\n';
    cout << "sizeof(match<>) == " << sizeof(m0) << '\n';
    cout << "sizeof(match<int>) == " << sizeof(m1) << '\n';
    cout << "sizeof(match<double>) == " << sizeof(m2) << '\n';

    match<boost::reference_wrapper<int> > mr;       // should compile
    match<boost::reference_wrapper<int> > mr2(3);   // should compile
    mr = mr2;
    match<boost::reference_wrapper<int> > mr3(mr);  // should compile
    mr2 = mr3;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Match Policy tests
//
///////////////////////////////////////////////////////////////////////////////
void
match_policy_tests()
{
    match<>         m0;
    match<int>      m1;
    match<double>   m2;
    match_policy    mp;

    m0 = mp.no_match();     assert(!m0);
    m1 = mp.no_match();     assert(!m1);
    m0 = mp.empty_match();  assert(m0);
    m2 = mp.empty_match();  assert(m2);

    m1 = mp.create_match(5, 100, 0, 0);
    m2 = mp.create_match(5, 10.5, 0, 0);

    mp.concat_match(m1, m2);
    assert(m1.length() == 10);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    match_tests();
    match_policy_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

