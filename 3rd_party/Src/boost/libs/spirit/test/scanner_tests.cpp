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
#include <list>

using namespace std;

#include "boost/spirit/core.hpp"
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Scanner tests
//
///////////////////////////////////////////////////////////////////////////////
struct to_upper_iter_policy : public iteration_policy {

    char filter(char ch) const
    { return toupper(ch); }
};

inline bool test_isspace(char c)
{
    using namespace std; return isspace(c);
}

inline bool test_islower(char c)
{
    using namespace std; return islower(c);
}

struct skip_white_iter_policy : public iteration_policy {

    template <typename ScannerT>
    void
    advance(ScannerT const& scan) const
    {
        do
            ++scan.first;
        while (!at_end(scan) && test_isspace(get(scan)));
    }
};

void
scanner_tests()
{
    char const* cp = "The Big Brown Fox Jumped \n\tOver The Lazy Dog's Back";
    char const* cp_first = cp;
    char const* cp_last = cp + strlen(cp);

    scanner<char const*>
        pp1(cp_first, cp_last);

    //  compile check only...
    scanner<> spp1(pp1);
    scanner<> spp2(pp1);
    //    spp1 = spp2;
    //  compile check only...

    while (!pp1.at_end())
    {
        cout << *pp1;
        ++pp1;
    }
    cout << '\n';
    cp_first = cp;

#if (defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)) \
    || defined(BOOST_INTEL_CXX_VERSION)
#else
//  VC++6 Cannot handle iterator traits properly

    list<char>              li(cp_first, cp_last);
    list<char>::iterator    li_first = li.begin();
    list<char>::iterator    li_last = li.end();

    scanner<list<char>::iterator>
        pp2(li_first, li_last);

    while (!pp2.at_end())
    {
        cout << *pp2;
        ++pp2;
    }
    cout << '\n';
    li_first = li.begin();

#endif

    scanner<char const*, scanner_policies<to_upper_iter_policy> >
        pp3(cp_first, cp_last);

    while (!pp3.at_end())
    {
        cout << *pp3;
        assert(!test_islower(*pp3));
        ++pp3;
    }
    cout << '\n';
    cp_first = cp;

    scanner<char const*, scanner_policies<skip_white_iter_policy> >
        pp4(cp_first, cp_last);

    //  compile check only...
    pp1.change_policies(scanner_policies<skip_white_iter_policy>());
    //  compile check only...

    while (!pp4.at_end())
    {
        cout << *pp4;
        assert(!test_isspace(*pp4));
        ++pp4;
    }
    cout << '\n';
    cp_first = cp;

    cout << "sizeof(scanner<>) == " << sizeof(scanner<>) << '\n';

    parse_info<> pi = parse("12abcdefg12345ABCDEFG789", +digit_p, alpha_p);
    assert(pi.hit);
    assert(pi.full);

    pi = parse("abcdefg12345ABCDEFG789", +digit_p, alpha_p);
    assert(pi.hit);
    assert(pi.full);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    scanner_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

