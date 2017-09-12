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

void force_division_by_zero()
{
    // unit test framework can catch operating system signals
    BOOST_CHECKPOINT("About to force division by zero!");
    int i = 1, j = 0;
    i = i / j;
}

void infinite_loop()
{
    // unit test framework can break infinite loops by timeout
#ifdef __unix  // don't have timeout on other platforms
    BOOST_CHECKPOINT("About to enter an infinite loop!");
    while(1);
#else
    BOOST_MESSAGE( "Timeout support is not implemented on your platform" );
#endif
}

test_suite*
init_unit_test_suite( int argc, char * argv[] ) {
    test_suite* test= BOOST_TEST_SUITE( "Unit test example 2" );

    test->add( BOOST_TEST_CASE( &force_division_by_zero ) );
    test->add( BOOST_TEST_CASE( &infinite_loop ), 0, /* timeout */ 2 );

    return test; 
}

// EOF
