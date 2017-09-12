/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 1998-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if defined (__BORLANDC__)
#define _RWSTD_COMPILE_INSTANTIATE // Borland Workaround
#endif

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

//#define BOOST_SPIRIT_DEBUG
#include "boost/spirit/core.hpp"
#include "boost/ref.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Actions tests
//
///////////////////////////////////////////////////////////////////////////////
std::string s1;
std::string s2;

void a1(char ch)
{
    s1 += ch;
}

void a2(char const* first, char const* last)
{
    s2 = std::string(first, last);
}

void
actions_tests()
{
    {
        char const* cp = "abcdefghihklmnop";
        char const* cp_first = cp;
        char const* cp_last = cp + strlen(cp);

        scanner<char const*> scan(cp_first, cp_last);
        match<> hit;

        hit = (+alpha_p[&a1])[&a2].parse(scan);
        assert(hit);
        assert(scan.first == scan.last);
        assert(s1 == cp);
        assert(s1 == s2);
        scan.first = cp;

        std::string s3;
        std::string s4;

        hit = (+alpha_p[append(s4)])[assign(s3)].parse(scan);
        assert(hit);
        assert(scan.first == scan.last);
        assert(s3 == s2);
        assert(s3 == s4);
    }

    using boost::ref;

    {
        char ch;
        char const* cp = "jj";
        char const* cp_first = cp;
        char const* cp_last = cp + strlen(cp);
        scanner<char const*> scan(cp_first, cp_last);

        match<> hit = (anychar_p[assign(ch)] >> ch_p(ref(ch))).parse(scan);
        assert(hit);
        assert(scan.first == scan.last);
    }

    {
        char const* cp = "abrakadabra";
        char const* cp_first = cp;
        char const* cp_last = cp + strlen(cp);
        scanner<char const*> scan(cp_first, cp_last);

        match<> hit = (str_p(ref(cp_first), ref(cp_last))).parse(scan);
        assert(hit);
        assert(scan.first == scan.last);
    }

    {
        char a_ = 'a';
        char j_ = 'j';
        char const* cp = "abcdefghijklmn";
        char const* cp_first = cp;
        char const* cp_last = cp + strlen(cp);
        scanner<char const*> scan(cp_first, cp_last);

        match<> hit = (+range_p(ref(a_), ref(j_))).parse(scan);
        assert(hit);
        assert(string(scan.first) == "klmn");
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    actions_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

