//  (C) Copyright Gennadiy Rozental 2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $$
//
//  Version     : $Id: parameterized_test_test.cpp,v 1.4 2003/02/15 21:52:37 rogeeff Exp $
//
//  Description : tests parameterized tests
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/test_tools.hpp>
#if !defined ( __GNUC__ ) || __GNUC__ > 2
#include <boost/test/detail/nullstream.hpp>
#endif

using namespace boost::unit_test_framework;

#include <list>
#include <iostream>
#include <boost/scoped_ptr.hpp>

//____________________________________________________________________________//

void test0( int i )
{
    BOOST_CHECK( i%2 == 0 );
}

//____________________________________________________________________________//

void test1( int i )
{
    BOOST_CHECK( i%2 == 0 );
    BOOST_REQUIRE( i%3 != 0 );
}

//____________________________________________________________________________//


#if defined(__BORLANDC__) && (__BORLANDC__ < 0x570)
#define BOOST_PARAM_TEST_CASE__( arg1, arg2, arg3 ) \
    boost::unit_test_framework::create_test_case<int*,int>( (arg1), std::string( "" ), (arg2), (arg3) )
#else
#define BOOST_PARAM_TEST_CASE__( arg1, arg2, arg3 ) BOOST_PARAM_TEST_CASE( arg1, arg2, arg3 )
#endif

int test_main( int, char* [] ) {
    unit_test_counter               num_of_failures;
    bool                            exception_caught;
#if !defined ( __GNUC__ ) || __GNUC__ > 2
    boost::onullstream              null_output;
#else
    boost::test_toolbox::output_test_stream null_output;
#endif
    boost::scoped_ptr<test_case>    test;  

    {
    unit_test_result_saver saver;
    unit_test_log::instance().set_log_stream( null_output );

    {
    int test_data[] = { 2, 2, 2 };
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 0 );
    BOOST_CHECK( !exception_caught );
    }

    {
    int test_data[] = { 1, 2, 2 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 1 );
    BOOST_CHECK( !exception_caught );
    }

    {
    int test_data[] = { 1, 1, 2 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 3 );
    BOOST_CHECK( !exception_caught );
    }

    {
    int test_data[] = { 1, 1, 1 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 6 );
    BOOST_CHECK( !exception_caught );
    }

    }
    {
    unit_test_result_saver saver;

    {
    int test_data[] = { 6, 6, 6 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 0 );
    BOOST_CHECK( !exception_caught );
    }

    }
    {
    unit_test_result_saver saver;

    {
    int test_data[] = { 0, 3, 9 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 2 );
    BOOST_CHECK( !exception_caught );
    }

    {
    int test_data[] = { 2, 3, 9 };
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 2 );
    BOOST_CHECK( exception_caught );
    }

    {
    int test_data[] = { 3, 2, 6 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );
    test->run();

    unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    BOOST_CHECK( num_of_failures == 1 );
    BOOST_CHECK( exception_caught );
    }

    }
    unit_test_log::instance().set_log_stream( std::cout );

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: parameterized_test_test.cpp,v $
//  Revision 1.4  2003/02/15 21:52:37  rogeeff
//  mingw ostream fix
//
//  Revision 1.3  2002/12/09 05:16:10  rogeeff
//  switched to use unit_test_result_saver for internal testing
//
//  Revision 1.2  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
