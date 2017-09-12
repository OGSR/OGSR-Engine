/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 1998-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>
#include <iostream>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
struct handler
{
    template <typename ScannerT, typename ErrorT>
    error_status<>
    operator()(ScannerT const& /*scan*/, ErrorT const& /*error*/) const
    {
        cout << "exception caught...Test concluded successfully" << endl;
        return error_status<>(error_status<>::fail);
    }
};

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tExceptions Test...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    assertion<int>  expect(0);
    guard<int>      my_guard;

    bool r =
        parse("abcx",
            my_guard(ch_p('a') >> 'b' >> 'c' >> expect( ch_p('d') ))
            [
                handler()
            ]
        ).full;

    assert(!r);
    return 0;
}

