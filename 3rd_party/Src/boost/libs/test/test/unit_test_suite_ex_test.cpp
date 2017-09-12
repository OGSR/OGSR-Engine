//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: unit_test_suite_ex_test.cpp,v $
//
//  Version     : $Id: unit_test_suite_ex_test.cpp,v 1.9 2003/02/15 21:49:58 rogeeff Exp $
//
//  Description : tests an extentions to Unit Test Framework
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_suite_ex.hpp>
#include <boost/test/test_tools.hpp>

using boost::unit_test_framework::test_suite;

#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
// BOOST
#include <boost/bind.hpp>
#endif

#include <boost/function.hpp>
using namespace boost;

// STL
#include <list>
#include <utility>

//____________________________________________________________________________//

void test0()
{
    BOOST_MESSAGE( "Hello there" );
}

//____________________________________________________________________________//

void test1( int arg )
{
    BOOST_CHECK( (arg & 0x80) == 0 );
}

//____________________________________________________________________________//

void test2( int arg, int mask )
{
    BOOST_CHECK( (arg & mask) != 0 );
}

//____________________________________________________________________________//

struct sub_test_suite : public test_suite {
    typedef std::list<std::pair<int,int> > mask_list_type;
    sub_test_suite()
    {
        parameters_list.push_back( 1 );
        parameters_list.push_back( 5 );
        parameters_list.push_back( 6 );
        parameters_list.push_back( 7 );
        parameters_list.push_back( 15 );

        masks_list.push_back( std::make_pair( 0x01, 1 ) );
        masks_list.push_back( std::make_pair( 0x04, 1 ) );
        masks_list.push_back( std::make_pair( 0x80, 5 ) );


#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
        for( mask_list_type::iterator it = masks_list.begin(); it != masks_list.end(); ++it ) {
            function1<void,int> fct = bind( &test2, _1, it->first );
            add( BOOST_PARAM_TEST_CASE( fct, parameters_list.begin(), parameters_list.end() ), it->second );
        }
#endif

        function1<void,int> fct = &test1;
        add( BOOST_PARAM_TEST_CASE( fct, parameters_list.begin(), parameters_list.end() ) );
    }

    
    std::list<int> parameters_list;
    mask_list_type masks_list; // mask/"num of errors expected" list
};

//____________________________________________________________________________//

test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
    test_suite* test = BOOST_TEST_SUITE("unit_test_suite extensions test");

    function0<void> fct1 = &test0;
    test->add( BOOST_TEST_CASE( fct1 ) );

#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
    function0<void> fct2 = bind( &test2, 12345, 0xcdf );
    test->add( BOOST_TEST_CASE( fct2 ) );
#endif

    test->add( new sub_test_suite );

    return test;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: unit_test_suite_ex_test.cpp,v $
//  Revision 1.9  2003/02/15 21:49:58  rogeeff
//  borland warning fix
//
//  Revision 1.8  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
