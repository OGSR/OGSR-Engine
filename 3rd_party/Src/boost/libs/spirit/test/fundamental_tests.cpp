/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  Fundamental meta sublayer tests
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>

#include <boost/static_assert.hpp>

#include "boost/spirit/core.hpp"

using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  node_count_tests
//
///////////////////////////////////////////////////////////////////////////////
void
node_count_tests()
{
// simple types
    typedef chlit<char> plain_t;
    typedef optional<chlit<char> > optional_t;
    typedef action<chlit<char>, assign_actor<char> > action_t;
    typedef sequence<chlit<char>, anychar_parser> sequence_t;

    BOOST_STATIC_ASSERT(1 == node_count<plain_t>::value);
    BOOST_STATIC_ASSERT(2 == node_count<optional_t>::value);
    BOOST_STATIC_ASSERT(2 == node_count<action_t>::value);
    BOOST_STATIC_ASSERT(3 == node_count<sequence_t>::value);

// more elaborate types
    typedef sequence<sequence<plain_t, action_t>, plain_t> sequence2_t;
    typedef sequence<plain_t, sequence<action_t, plain_t> > sequence3_t;

    BOOST_STATIC_ASSERT(6 == node_count<sequence2_t>::value);
    BOOST_STATIC_ASSERT(6 == node_count<sequence3_t>::value);
}

///////////////////////////////////////////////////////////////////////////////
//
//  leaf_count_tests
//
///////////////////////////////////////////////////////////////////////////////
void
leaf_count_tests()
{
// simple types
    typedef chlit<char> plain_t;
    typedef optional<chlit<char> > optional_t;
    typedef action<chlit<char>, assign_actor<char> > action_t;
    typedef sequence<chlit<char>, anychar_parser> sequence_t;

    BOOST_STATIC_ASSERT(1 == leaf_count<plain_t>::value);
    BOOST_STATIC_ASSERT(1 == leaf_count<optional_t>::value);
    BOOST_STATIC_ASSERT(1 == leaf_count<action_t>::value);
    BOOST_STATIC_ASSERT(2 == leaf_count<sequence_t>::value);

// more elaborate types
    typedef sequence<sequence<plain_t, action_t>, plain_t> sequence2_t;
    typedef sequence<plain_t, sequence<action_t, plain_t> > sequence3_t;

    BOOST_STATIC_ASSERT(3 == leaf_count<sequence2_t>::value);
    BOOST_STATIC_ASSERT(3 == leaf_count<sequence3_t>::value);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    node_count_tests();
    leaf_count_tests();

    cout << "Tests concluded successfully\n";
    return 0;
}

