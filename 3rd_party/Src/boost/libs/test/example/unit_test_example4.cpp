//  (C) Copyright Gennadiy Rozental & Ullrich Koethe 2001.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org for most recent version including documentation.

// Boost.Test
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

// STL
#include <vector>
#include <string>

void check_string( std::string const& s )
{
    // reports 'error in "check_string": test s.substr( 0, 3 ) == "hdr" failed [actual_value != hdr]'
    BOOST_CHECK_EQUAL( s.substr( 0, 3 ), "hdr" );
}

namespace {
    std::string const params[] = { "hdr1 ", "hdr2", "3  " };
}

test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
    test_suite* test= BOOST_TEST_SUITE( "Unit test example 4" );

    test->add( BOOST_PARAM_TEST_CASE( &check_string, (std::string const*)params, params+3 ), 1 );

    return test; 
}

// EOF
