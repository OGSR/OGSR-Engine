//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: unit_test_main.cpp,v $
//
//  Version     : $Id: unit_test_main.cpp,v 1.9 2003/02/13 08:43:01 rogeeff Exp $
//
//  Description : main function implementation for Unit Test Framework
// ***************************************************************************

#include <boost/test/unit_test.hpp>   // for unit_test framework
#include <boost/test/unit_test_result.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
#include <boost/test/detail/unit_test_monitor.hpp>

// BOOST
#include <boost/scoped_ptr.hpp>

// STL
#include <string>
#include <iostream>                   // for cout, cerr

extern boost::unit_test_framework::test_suite* init_unit_test_suite( int argc, char* argv[] );  // prototype for user's test suite init function

// ************************************************************************** //
// **************                 unit test main               ************** //
// ************************************************************************** //

int
main( int argc, char* argv[] )
{
    using namespace boost::unit_test_framework;

    bool    no_result_code;
    bool    print_build_info;

    // set the log level
    unit_test_log::instance().set_log_threshold_level_by_name( retrieve_framework_parameter( LOG_LEVEL, &argc, argv ) );

    // set the log/report format
    std::string output_format = retrieve_framework_parameter( OUTPUT_FORMAT, &argc, argv );
    
    if( output_format.empty() ) {
        unit_test_log::instance().set_log_format( retrieve_framework_parameter( LOG_FORMAT, &argc, argv ) );
        unit_test_result::set_report_format( retrieve_framework_parameter( REPORT_FORMAT, &argc, argv ) );
    }
    else {
        unit_test_log::instance().set_log_format( output_format );
        unit_test_result::set_report_format( output_format );
    }

    // set the result code and build info flags
    no_result_code    = retrieve_framework_parameter( NO_RESULT_CODE, &argc, argv ) == "no";
    print_build_info  = retrieve_framework_parameter( BUILD_INFO, &argc, argv ) == "yes";

    // set catch_system_error switch
    detail::unit_test_monitor::catch_system_errors( retrieve_framework_parameter( CATCH_SYS_ERRORS, &argc, argv ) != "no" );

    // init master unit test suite
    boost::scoped_ptr<test_suite> test( init_unit_test_suite( argc, argv ) );
    if( !test ) {
        std::cerr << "*** Fail to initialize test suite" << std::endl;
        return -999;
    }

    // start testing
    unit_test_log::instance().start( print_build_info );
    unit_test_log::instance().header( test->size() );
    test->run();
    unit_test_log::instance().finish( test->size() );

    // report results
    unit_test_result::instance().report( retrieve_framework_parameter( REPORT_LEVEL, &argc, argv ), std::cerr );

    // return code
    return no_result_code ? boost::exit_success : unit_test_result::instance().result_code();
}

// ***************************************************************************
//  Revision History :
//  
//  $Log: unit_test_main.cpp,v $
//  Revision 1.9  2003/02/13 08:43:01  rogeeff
//  log/report format option accepted,
//  report selection logic moved in class method
//  log/report separated into different stream stdout/stderr
//
//  Revision 1.8  2002/12/08 18:11:39  rogeeff
//  catch system errors switch added
//  switch back to scoped_ptr instead of raw test_suite pointer
//
//  Revision 1.7  2002/11/02 20:04:42  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
