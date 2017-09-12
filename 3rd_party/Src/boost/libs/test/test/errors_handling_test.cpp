//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes 2001.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: errors_handling_test.cpp,v $
//
//  Version     : $Id: errors_handling_test.cpp,v 1.11 2003/02/15 21:49:57 rogeeff Exp $
//
//  Description : tests an ability of Unit Test Framework to catch all kinds
//  of test errors in a user code and properly report it.
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
using namespace boost::unit_test_framework;
using namespace boost::test_toolbox;

// STL
#include <iostream>

//____________________________________________________________________________//

namespace {
    enum error_type_enum {
        et_begin,
        et_none = et_begin,
        et_user,
        et_cpp_exception,
        et_system,
        et_fatal_user,
        et_fatal_system,
        et_end
    } error_type;

    char const* error_type_name[] = { "no error", "user error", "cpp exception", " system error", "fatal user error", "fatal system error" };

    int divide_by_zero = 0;

    // will cause an error coresponding to the current error_type;
    void error_on_demand()
    {
        switch( error_type ) {
        case et_none:
            BOOST_MESSAGE( "error_on_demand() BOOST_MESSAGE" );
            break;

        case et_user:
            unit_test_result::instance().increase_expected_failures();
            BOOST_ERROR( "error_on_demand() BOOST_ERROR" );
            break;

        case et_fatal_user:
            unit_test_result::instance().increase_expected_failures();
            BOOST_CRITICAL_ERROR( "error_on_demand() BOOST_CRITICAL_ERROR" );

            BOOST_ERROR( "Should never reach this code!" );
            break;

        case et_cpp_exception:
            BOOST_CHECKPOINT( "error_on_demand() throw runtime_error" );
            throw std::runtime_error( "test std::runtime error what() message" );

        case et_system:
            BOOST_CHECKPOINT( "error_on_demand() divide by zero" );
            divide_by_zero = 1 / divide_by_zero;
            break;

        case et_fatal_system:
            BOOST_CHECKPOINT( "error_on_demand() write to an invalid address" );
            {
                int* p = 0;
                *p = 0;

                BOOST_ERROR( "Should never reach this code!" );
            }
            break;

        default:
            BOOST_ERROR( "Should never reach this code!" );
        }
        return;
    }

    enum test_case_type_enum {
        tct_begin,
        tct_free_function = tct_begin,
        tct_user_test_case,
        tct_param_free_function,
        tct_param_user_test_case,
        tct_end
    } test_case_type;

    char const* test_case_type_name[] = { "free function",
                                          "user test case",
                                          "parameterized free function",
                                          "parameterized user test case"
    };

    //  simulated user classes to be tested  --------------------------------//

    // user test cases   ----------------------------------------------------//

    struct bad_test
    {
        void test()
        {
            BOOST_MESSAGE( "(user test case)" );
            error_on_demand();
        }
        void test_param( int )
        {
            BOOST_MESSAGE( "(parameterized user test case)" );
            error_on_demand();
        }
    };

    //  free function tests  ---------------------------------------------------//

    void bad_function()
    {
        BOOST_MESSAGE( "(free function)" );
        error_on_demand();
    }

    void bad_function_param( int )
    {
        BOOST_MESSAGE( "(parameterized free function)" );
        error_on_demand();
    }

    int params[] = { 0 };

}  // unnamed namespace

//____________________________________________________________________________//

int
test_main( int argc, char * argv[] )
{
    bool match_or_save = retrieve_framework_parameter( SAVE_TEST_PATTERN, &argc, argv ) != "yes";

    std::string pattern_file_name( argc > 1 ? argv[1] : "errors_handling_test.pattern" );

    output_test_stream output( pattern_file_name, match_or_save );

    unit_test_log::instance().set_log_stream( output );

    boost::shared_ptr<bad_test> bad_test_instance( new bad_test );

    // for each log level
    for( log_level level = log_successful_tests;
         level           <= log_nothing;
         level           = static_cast<log_level>(level+1) )
    {
        unit_test_log::instance().set_log_threshold_level( level );

        // for each error type
        for( error_type = et_begin;
             error_type != et_end;
             error_type = static_cast<error_type_enum>(error_type+1) )
        {
            // for each error location
            for( test_case_type = tct_begin;
                 test_case_type != tct_end;
                 test_case_type = static_cast<test_case_type_enum>(test_case_type+1) )
            {
                output << "\n===========================\n\n"
                       << "log level: "       << int(level) << ';'
                       << " error type: "     << error_type_name[error_type] << ';'
                       << " test case type: " << test_case_type_name[test_case_type] << ';'<< std::endl;

                // In typical user code, multiple test cases would be added to a single
                // test suite.  But for testing the unit test code itself, it is easier
                // to isolate each case in its own test suite.
                test_suite test( "Errors handling test" );
                switch( test_case_type ) {
                case tct_free_function:
                    test.add( BOOST_TEST_CASE( &bad_function ) );
                    break;
                case tct_user_test_case:
                    test.add( BOOST_CLASS_TEST_CASE( &bad_test::test, bad_test_instance ) );
                    break;
                case tct_param_free_function:
// Borland bug workaround
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x570)
                    test.add( boost::unit_test_framework::create_test_case<int*,int>( &bad_function_param, std::string( "bad_function_param" ), (int*)params, params+1 ) );
#else
                    test.add( BOOST_PARAM_TEST_CASE( &bad_function_param, (int*)params, params+1 ) );
#endif
                    break;
                case tct_param_user_test_case:
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x570)
                    test.add( boost::unit_test_framework::create_test_case<bad_test,int*,int>( &bad_test::test_param, std::string( "bad_test::test_param" ),  bad_test_instance, (int*)params, params+1 ) );
#else
                    test.add( BOOST_PARAM_CLASS_TEST_CASE( &bad_test::test_param, bad_test_instance, (int*)params, params+1 ) );
#endif
                    break;
                default:
                    continue;
                }

                { 
                    unit_test_result_saver saver;
                    unit_test_log::instance().start();
                    unit_test_log::instance().header( 1 );
                    test.run();
                    unit_test_log::instance().finish( 1 );
                }

                unit_test_log::instance().set_log_threshold_level( log_all_errors );
                BOOST_CHECK( output.match_pattern() );
                unit_test_log::instance().set_log_threshold_level( level );
            }
        }
    }

    unit_test_result::instance().short_report( output );
    output.match_pattern();

    return 0;
} // main

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//
//  $Log: errors_handling_test.cpp,v $
//  Revision 1.11  2003/02/15 21:49:57  rogeeff
//  borland warning fix
//
//  Revision 1.10  2003/02/13 08:47:05  rogeeff
//  *** empty log message ***
//
//  Revision 1.9  2002/12/09 05:14:45  rogeeff
//  switch to use unit_test_result_saver for internal testing
//
//  Revision 1.8  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF

