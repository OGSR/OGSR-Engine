//  (C) Copyright Gennadiy Rozental 2001.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org for most recent version including documentation.

// Boost.Test
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
using boost::unit_test_framework::test_suite;

// BOOST
#include <boost/functional.hpp>
#include <boost/static_assert.hpp>

// STL
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>
#include <list>

//____________________________________________________________________________//

template<int n>
struct pow10 {
    BOOST_STATIC_CONSTANT( unsigned long, value = 10*pow10<n-1>::value );
};

template<>
struct pow10<0> {
    BOOST_STATIC_CONSTANT( unsigned long, value = 1 );
};

//____________________________________________________________________________//

template<int AlphabetSize>
class hash_function {
public:
    BOOST_STATIC_ASSERT( AlphabetSize <= 5 );

    explicit        hash_function( std::string const& alphabet )
    : m_alphabet( alphabet )
    {
        if( m_alphabet.size() != AlphabetSize )
            throw std::length_error( "Wrong alphabet size" );

        std::sort( m_alphabet.begin(), m_alphabet.end() );

        if( std::adjacent_find( m_alphabet.begin(), m_alphabet.end() ) != m_alphabet.end() )
            throw std::logic_error( "Duplicate characters in alphabet" );
    }

    unsigned long   operator()( std::string const& arg )
    {
        m_result = 0;

        if( arg.length() > 8 )
            throw std::length_error( "Wrong argument size" );

        std::string::const_iterator it = std::find_if( arg.begin(), arg.end(), 
                                                       std::bind1st( boost::mem_fun( &hash_function::helper_ ), this ) );

        if( it != arg.end() )
            throw std::out_of_range( std::string( "Invalid character " ) + *it );

        return m_result;
    }

private:
    bool            helper_( char c )
    {       
        std::string::const_iterator it = std::find( m_alphabet.begin(), m_alphabet.end(), c );
        
        if( it == m_alphabet.end() )
            return true;

        m_result += pow10_( it - m_alphabet.begin() );

        return false;
    }

    unsigned long   pow10_( int i ) {
        switch( i ) {
        case 0: return pow10<0>::value;
        case 1: return pow10<1>::value;
        case 2: return pow10<2>::value;
        case 3: return pow10<3>::value;
        case 4: return pow10<4>::value;
        default: return 0;
        }
    }

    // Data members
    std::string     m_alphabet;
    unsigned long   m_result;
};

//____________________________________________________________________________//

struct hash_function_test_data {
    std::string     orig_string;
    unsigned long   exp_value;

    friend std::istream& operator>>( std::istream& istr, hash_function_test_data& test_data )
    {
        std::istream& tmp = istr >> test_data.orig_string;
        return  !tmp ? tmp : istr >> test_data.exp_value;
    }
};

//____________________________________________________________________________//

class hash_function_tester {
public:
    explicit        hash_function_tester( std::string const& alphabet )
    : m_function_under_test( alphabet ) {}

    void            test( hash_function_test_data const& test_data )
    {
        if( test_data.exp_value == (unsigned long)-1 )
            BOOST_CHECK_THROW( m_function_under_test( test_data.orig_string ), std::length_error )
        else if( test_data.exp_value == (unsigned long)-2 )
            BOOST_CHECK_THROW( m_function_under_test( test_data.orig_string ), std::out_of_range )
        else {
            BOOST_MESSAGE( "Testing: " << test_data.orig_string );
            BOOST_CHECK_EQUAL( m_function_under_test( test_data.orig_string ), test_data.exp_value );
        }
    }

private:
    hash_function<4> m_function_under_test;
};

//____________________________________________________________________________//

struct massive_hash_function_test : test_suite {
    massive_hash_function_test() {
        std::string alphabet;
        std::cout << "Enter alphabet (4 characters without delimeters)\n";
        std::cin >> alphabet;

        boost::shared_ptr<hash_function_tester> instance( new hash_function_tester( alphabet ) );

        std::cout << "\nEnter test data in a format [string] [value] to check correct calculation\n";
        std::cout << "Enter test data in a format [string] -1 to check long string validation\n";
        std::cout << "Enter test data in a format [string] -2 to check invalid argument string validation\n";

        while( !std::cin.eof() ) {
            hash_function_test_data test_data;
            
            if( !(std::cin >> test_data) )
                break;

            m_test_data.push_back( test_data );
        }

        add( BOOST_PARAM_CLASS_TEST_CASE( &hash_function_tester::test, instance, m_test_data.begin(), m_test_data.end() ) );
    }

    std::list<hash_function_test_data> m_test_data;
};

test_suite*
init_unit_test_suite( int argc, char * argv[] ) {
    std::auto_ptr<test_suite> test( BOOST_TEST_SUITE( "Unit test example 5" ) );
  
    try  {
        test->add( new massive_hash_function_test );
    } catch( std::logic_error const& ex ) {
        std::cout << "Test suite fail to create instance of hash function: " << ex.what() << std::endl;

        return (test_suite*)0;
    }

    return test.release(); 
}

// EOF
