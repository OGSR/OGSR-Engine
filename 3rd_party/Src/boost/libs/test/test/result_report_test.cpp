//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: result_report_test.cpp,v $
//
//  Version     : $Id: result_report_test.cpp,v 1.8 2003/02/15 21:52:37 rogeeff Exp $
//
//  Description : tests Unit Test Framework reporting facilities against
//  pattern file
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
#if !defined ( __GNUC__ ) || __GNUC__ > 2
#include <boost/test/detail/nullstream.hpp>
#endif
using boost::test_toolbox::output_test_stream;
using namespace boost::unit_test_framework;

// BOOST
#include <boost/lexical_cast.hpp>

// STL
#include <iostream>
#include <string>
#include <utility>
#include <list>

//____________________________________________________________________________//

void good_foo() {}
void bad_foo()  { 
#if !defined ( __GNUC__ ) || __GNUC__ > 2
    boost::onullstream null_stream;
#else
    output_test_stream null_stream;
#endif

    unit_test_log::instance().set_log_stream( null_stream );
    BOOST_ERROR( "Sure" );
    unit_test_log::instance().set_log_stream( std::cout );
}

//____________________________________________________________________________//

void check( output_test_stream& output, std::string const& report_format )
{
    unit_test_result::instance().set_report_format( report_format );

    unit_test_result::instance().confirmation_report( output );
    output << "*************************************************************************\n\n";
    BOOST_CHECK( output.match_pattern() );

    unit_test_result::instance().short_report( output );
    output << "*************************************************************************\n\n";
    BOOST_CHECK( output.match_pattern() );

    unit_test_result::instance().detailed_report( output );
    output << "*************************************************************************\n\n";
    BOOST_CHECK( output.match_pattern() );
}

//____________________________________________________________________________//

void check( output_test_stream& output )
{
    check( output, "HRF" );
    check( output, "XML" );
}

//____________________________________________________________________________//

int 
test_main( int argc, char* argv[] ) 
{
    bool match_or_save = retrieve_framework_parameter( SAVE_TEST_PATTERN, &argc, argv ) != "yes";
    std::string pattern_file_name( argc > 1 ? argv[1] : "result_report_test.pattern" );

    output_test_stream output( pattern_file_name, match_or_save );
  
    test_suite* ts_0 = BOOST_TEST_SUITE( "0 test cases inside" );
    
    test_suite* ts_1 = BOOST_TEST_SUITE( "1 test cases inside" );
        ts_1->add( BOOST_TEST_CASE( good_foo ) );

    test_suite* ts_1b = BOOST_TEST_SUITE( "1 bad test cases inside" );
        ts_1b->add( BOOST_TEST_CASE( bad_foo ), 1 );

    test_suite* ts_2 = BOOST_TEST_SUITE( "2 test cases inside" );
        ts_2->add( BOOST_TEST_CASE( good_foo ) );
        ts_2->add( BOOST_TEST_CASE( bad_foo ), 1 );

    test_suite* ts_3 = BOOST_TEST_SUITE( "3 test cases inside" );
        ts_3->add( BOOST_TEST_CASE( good_foo ) );
        ts_3->add( BOOST_TEST_CASE( good_foo ) );
        ts_3->add( BOOST_TEST_CASE( bad_foo ), 1 );

    test_suite ts_main( "Fake Test Suite Hierarchy" );
        ts_main.add( ts_0 );
        ts_main.add( ts_1 );
        ts_main.add( ts_2 );
        ts_main.add( ts_3 );

    check( output );

    ts_0->run();
    check( output );

    ts_1->run();
    check( output );

    unit_test_result::instance().increase_expected_failures();
    ts_2->run();
    check( output );

    unit_test_result::instance().increase_expected_failures();
    ts_1b->run();
    check( output );

    unit_test_result::instance().increase_expected_failures();
    ts_3->run();
    check( output );

    unit_test_result::instance().increase_expected_failures( 2 );
    ts_main.run();

    check( output );

    std::string output_format = retrieve_framework_parameter( OUTPUT_FORMAT, &argc, argv );
    
    if( output_format.empty() ) {
        unit_test_result::set_report_format( retrieve_framework_parameter( REPORT_FORMAT, &argc, argv ) );
    }

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: result_report_test.cpp,v $
//  Revision 1.8  2003/02/15 21:52:37  rogeeff
//  mingw ostream fix
//
//  Revision 1.7  2003/02/13 08:47:07  rogeeff
//  *** empty log message ***
//
//  Revision 1.6  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
