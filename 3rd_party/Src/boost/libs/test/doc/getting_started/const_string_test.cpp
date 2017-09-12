//  (C) Copyright Gennadiy Rozental 2001.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

#include <time.h>

#include <const_string.hpp>
#include <const_string_traits.hpp>
#include <const_string_compare.hpp>
#include <const_string_oper.hpp>
#include <const_string_io.hpp>

using common_layer::const_string;

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test_framework;

#include <iomanip>
#include <iostream>
#include <stdexcept>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::strlen; using ::strcmp; using ::strncmp; }
# endif

void constructors_test() {
    const_string cs0( "" );
    BOOST_CHECK_EQUAL( cs0.length(), (size_t)0 );
    BOOST_CHECK_EQUAL( cs0.begin(), "" );
    BOOST_CHECK_EQUAL( cs0.end(), "" );
    BOOST_CHECK( cs0.is_empty() );

    const_string cs01( NULL );
    BOOST_CHECK_EQUAL( cs01.length(), (size_t)0 );
    BOOST_CHECK_EQUAL( cs01.begin(), "" );
    BOOST_CHECK_EQUAL( cs01.end(), "" );
    BOOST_CHECK( cs01.is_empty() );

    const_string cs1( "test_string" );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test_string" ), 0 );
    BOOST_CHECK_EQUAL( cs1.length(), std::strlen("test_string") );

    std::string  s( "test_string" );
    const_string cs2( s );
    BOOST_CHECK_EQUAL( std::strcmp( cs2.data(), "test_string" ), 0 );

    const_string cs3( cs1 );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test_string" ), 0 );

    const_string cs4( "test_string", 4 );
    BOOST_CHECK_EQUAL( std::strncmp( cs4.data(), "test", cs4.length() ), 0 );

    const_string cs5( s.data(), s.data() + s.length() );
    BOOST_CHECK_EQUAL( std::strncmp( cs5.data(), "test_string", cs5.length() ), 0 );

    const_string cs_array[] = { "str1", "str2" };

    BOOST_CHECK_EQUAL( cs_array[0], "str1" );
    BOOST_CHECK_EQUAL( cs_array[1], "str2" );
}

void data_access_test() {
    const_string cs1( "test_string" );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test_string" ), 0 );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), cs1 ), 0 );

    BOOST_CHECK_EQUAL( cs1[(size_t)0], 't' );
    BOOST_CHECK_EQUAL( cs1[(size_t)4], '_' );
    BOOST_CHECK_EQUAL( cs1[cs1.length()-1], 'g' );

    BOOST_CHECK_EQUAL( cs1[(size_t)0], cs1.at( 0 ) );
    BOOST_CHECK_EQUAL( cs1[(size_t)2], cs1.at( 5 ) );
    BOOST_CHECK_EQUAL( cs1.at( cs1.length() - 1 ), 'g' );

    BOOST_CHECK_THROW( cs1.at( cs1.length() ), std::out_of_range );

    BOOST_CHECK_EQUAL( common_layer::first_char()( cs1  ), 't' );
    BOOST_CHECK_EQUAL( common_layer::last_char()( cs1  ) , 'g' );
}

void length_test() {
    const_string cs1;

    BOOST_CHECK_EQUAL( cs1.length(), (size_t)0 );
    BOOST_CHECK( cs1.is_empty() );

    cs1 = "";
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)0 );
    BOOST_CHECK( cs1.is_empty() );

    cs1 = "test_string";
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)11 );

    cs1.erase();
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)0 );
    BOOST_CHECK_EQUAL( cs1.data(), "" );

    cs1 = const_string( "test_string", 4 );
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)4 );

    cs1.resize( 5 );
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)4 );

    cs1.resize( 3 );
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)3 );

    cs1.rshorten();
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)2 );
    BOOST_CHECK_EQUAL( cs1[(size_t)0], 't' );

    cs1.lshorten();
    BOOST_CHECK_EQUAL( cs1.length(), (size_t)1 );
    BOOST_CHECK_EQUAL( cs1[(size_t)0], 'e' );

    cs1.lshorten();
    BOOST_CHECK( cs1.is_empty() );
    BOOST_CHECK_EQUAL( cs1.data(), "" );

    cs1 = "test_string";
    cs1.lshorten( 11 );
    BOOST_CHECK( cs1.is_empty() );
    BOOST_CHECK_EQUAL( cs1.data(), "" );
}

void asignment_test() {
    const_string cs1;
    std::string  s( "test_string" );

    cs1 = "test";
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test" ), 0 );

    cs1 = s;
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test_string" ), 0 );

    cs1.assign( "test" );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test" ), 0 );

    const_string cs2( "test_string" );

    cs1.swap( cs2 );
    BOOST_CHECK_EQUAL( std::strcmp( cs1.data(), "test_string" ), 0 );
    BOOST_CHECK_EQUAL( std::strcmp( cs2.data(), "test" ), 0 );
}

void comparison_test() {
    const_string cs1( "test_string" );
    const_string cs2( "test_string" );
    std::string  s( "test_string" );

    BOOST_CHECK_EQUAL( cs1, "test_string" );
    BOOST_CHECK_EQUAL( "test_string", cs1 );
    BOOST_CHECK_EQUAL( cs1, cs2 );
    BOOST_CHECK_EQUAL( cs1, s );
    BOOST_CHECK_EQUAL( s  , cs1 );

    cs1.resize( 4 );

    BOOST_CHECK( cs1 != "test_string" );
    BOOST_CHECK( "test_string" != cs1 );
    BOOST_CHECK( cs1 != cs2 );
    BOOST_CHECK( cs1 != s );
    BOOST_CHECK( s   != cs1 );

    BOOST_CHECK_EQUAL( cs1, "test" );
}

void iterators_test() {
    const_string cs1( "test_string" );
    std::string  s;

    std::copy( cs1.begin(), cs1.end(), std::back_inserter( s ) );
    BOOST_CHECK_EQUAL( cs1, s );

    s.erase();

    std::copy( cs1.rbegin(), cs1.rend(), std::back_inserter( s ) );
    BOOST_CHECK_EQUAL( const_string( s ), "gnirts_tset" );
}

void traits_test() {
    const_string cs1( "1234567" );

    BOOST_CHECK( common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( common_layer::isNumber()( cs1 ) );

    cs1 = "asdRTY";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( !common_layer::isNumber()( cs1 ) );

    cs1 = "-145";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( common_layer::isNumber()( cs1 ) );

    cs1 = "+145e-3";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( !common_layer::isNumber()( cs1 ) );

    cs1 = "+145.3";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( common_layer::isNumber()( cs1 ) );

    cs1 = "145L";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( !common_layer::isNumber()( cs1 ) );

    cs1 = "145#";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( !common_layer::isNumber()( cs1 ) );

    cs1 = "145.34.45";

    BOOST_CHECK( !common_layer::isDigit()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlpha()( cs1 ) );
    BOOST_CHECK( !common_layer::isAlnum()( cs1 ) );
    BOOST_CHECK( !common_layer::isNumber()( cs1 ) );
}

void compare_test() {
    const_string cs1( "abcd" );
    const_string cs2( "abbdd" );
    const_string cs3( "abbde" );
    const_string cs4( "aBCd" );

    BOOST_CHECK( std::less<const_string>()( cs1, cs2 ) );
    BOOST_CHECK( std::less<const_string>()( cs2, cs3 ) );
    BOOST_CHECK( std::less<const_string>()( cs1, cs3 ) );
    BOOST_CHECK( common_layer::case_ins_eq()( cs1, cs4 ) );
}

void operations_test() {
    const_string cs1( "abcd   " );
    const_string cs2( "     abcd" );
    const_string cs3( "  abcd  " );

    BOOST_CHECK_EQUAL( common_layer::trim_left<const_string>()( cs2 ) , "abcd" );
    BOOST_CHECK_EQUAL( common_layer::trim_right<const_string>()( cs1 ), "abcd" );
    BOOST_CHECK_EQUAL( common_layer::trim<const_string>()( cs3 )      , "abcd" );
}

void io_test() {
    const_string cs1( "test_string" );
    cs1.rshorten( 7 );

    boost::output_test_stream ostr;

    ostr << std::setw( 6 ) << cs1;
    BOOST_CHECK( ostr.is_equal( "  test" ) );

    ostr << std::setw( 3 ) << cs1;
    BOOST_CHECK( ostr.is_equal( "test" ) );

    ostr << std::setw( 5 ) << std::setiosflags( std::ios_base::left ) << cs1;
    BOOST_CHECK( ostr.is_equal( "test " ) );
}

void search_test() {
    const_string cs( "test_string" );

    BOOST_CHECK_EQUAL( cs.find_first_of( 't' ), cs.begin() );
    BOOST_CHECK_EQUAL( cs.find_last_of( 't' ), cs.begin() + 6 );

    BOOST_CHECK_EQUAL( cs.find_first_of( "st" ), cs.begin() + 2 );
    BOOST_CHECK_EQUAL( cs.find_last_of( "st" ), cs.begin() + 5 );
}

test_suite*init_unit_test_suite( int argc, char* argv[] ) {
    test_suite* test= BOOST_TEST_SUITE("const_string test");

    test->add( BOOST_TEST_CASE( &constructors_test ) );
    test->add( BOOST_TEST_CASE( &data_access_test ) );
    test->add( BOOST_TEST_CASE( &length_test ) );
    test->add( BOOST_TEST_CASE( &asignment_test ) );
    test->add( BOOST_TEST_CASE( &comparison_test ) );
    test->add( BOOST_TEST_CASE( &iterators_test ) );
    test->add( BOOST_TEST_CASE( &traits_test ) );
    test->add( BOOST_TEST_CASE( &compare_test ) );
    test->add( BOOST_TEST_CASE( &operations_test ) );
    test->add( BOOST_TEST_CASE( &io_test ) );
    test->add( BOOST_TEST_CASE( &search_test ) );

    return test;
}

// EOF
