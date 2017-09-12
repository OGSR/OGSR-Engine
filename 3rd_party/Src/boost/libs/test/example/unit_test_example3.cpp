//  (C) Copyright Gennadiy Rozental & Ullrich Koethe 2001.
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
#include <boost/lexical_cast.hpp>

// STL
#include <functional>
#include <iostream>
#include <memory>

struct account {
    account()
    : m_amount(0.0)
    {}

    void deposit(double amount) { m_amount += amount; }
    void withdraw(double amount)
    {
        if(amount > m_amount)
        {
            throw std::runtime_error("You don't have that much money!");
        }
        m_amount -= amount;
    }
    double balance() const { return m_amount; }

private:
    double m_amount;
};

struct account_test {
    account_test( double init_value ) { m_account.deposit( init_value ); }

    account m_account;  // a very simple fixture

    void test_init()
    {
        // different kinds of non-critical tests
        // they report the error and continue

        // standard assertion
        // reports 'error in "account_test::test_init": test m_account.balance() >= 0.0 failed' on error
        BOOST_CHECK( m_account.balance() >= 0.0 );

        // customized assertion
        // reports 'error in "account_test::test_init": Initial balance should be more then 1, was actual_value' on error
        BOOST_CHECK_MESSAGE( m_account.balance() > 1.0,
                             "Initial balance should be more then 1, was " << m_account.balance() );

        // equality assertion (not very wise idea use equlality check on floating point values)
        // reports 'error in "account_test::test_init": test m_account.balance() == 5.0 failed [actual_value != 5]' on error
        BOOST_CHECK_EQUAL( m_account.balance(), 5.0 );

        // closeness assertion for floating-point numbers (symbol (==) used to mark closeness, (!=) to mark non closeness )
        // reports 'error in "account_test::test_init": test m_account.balance() (==) 10.0 failed [actual_value (!=) 10 (1e-010)]' on error
        BOOST_CHECK_CLOSE( m_account.balance(), 10.0, /* tolerance */ 1e-10 );
    }

    void test_deposit()
    {
        float curr_ballance = (float)m_account.balance();
        float deposit_value;

        std::cout << "Enter deposit value:\n";
        std::cin  >> deposit_value;

        m_account.deposit( deposit_value );

        // correct result validation; could fail due to rounding errors; use BOOST_CHECK_CLOSE instead
        // reports "test m_account.balance() == curr_ballance + deposit_value failed" on error
        BOOST_CHECK( m_account.balance() == curr_ballance + deposit_value );

        // different kinds of critical tests

        // reports 'fatal error in "account_test::test_deposit": test m_account.balance() >= 100.0 failed' on error
        BOOST_REQUIRE( m_account.balance() >= 100.0 );

        // reports 'fatal error in "account_test::test_deposit": Balance should be more than 500.1, was actual_value' on error
        BOOST_REQUIRE_MESSAGE( m_account.balance() > 500.1,
                               "Balance should be more than 500.1, was " << m_account.balance());

        // reports 'fatal error in "account_test::test_deposit": test std::not_equal_to<double>()(m_account.balance(), 999.9) failed
        //          for (999.9, 999.9)' on error
        BOOST_REQUIRE_PREDICATE( std::not_equal_to<double>(), 2, (m_account.balance(), 999.9) );

        // reports 'fatal error in "account_test::test_deposit": test close_at_tolerance<double>( 1e-9 )( m_account.balance(), 605.5)
        //          failed for (actual_value, 605.5)
        BOOST_REQUIRE_PREDICATE( close_at_tolerance<double>( 1e-9 ), 2, (m_account.balance(), 605.5) );
    }

    void test_withdraw()
    {
        float curr_ballance = (float)m_account.balance();

        m_account.withdraw(2.5);

        // correct result validation; could fail due to rounding errors; use BOOST_CHECK_CLOSE instead
        // reports "test m_account.balance() == curr_ballance - 2.5 failed" on error
        BOOST_CHECK( m_account.balance() == curr_ballance - 2.5 );

        // reports 'error in "account_test::test_withdraw": exception std::runtime_error is expected' on error
        BOOST_CHECK_THROW( m_account.withdraw( m_account.balance() + 1 ), std::runtime_error );
    }
};

struct account_test_suite : public test_suite {
    account_test_suite( double init_value ) : test_suite("account_test_suite") {
        // add member function test cases to a test suite
        boost::shared_ptr<account_test> instance( new account_test( init_value ) );

        add( BOOST_CLASS_TEST_CASE( &account_test::test_init, instance ), 4 );
        add( BOOST_CLASS_TEST_CASE( &account_test::test_deposit, instance ), 1 );
        add( BOOST_CLASS_TEST_CASE( &account_test::test_withdraw, instance ) );
    }
};

test_suite*
init_unit_test_suite( int argc, char * argv[] ) {
    std::auto_ptr<test_suite> test( BOOST_TEST_SUITE( "Unit test example 3" ) );

    try {
        if( argc < 2 )
            return (test_suite*)0;

        test->add( new account_test_suite( boost::lexical_cast<double>( argv[1] ) ) );
    }
    catch( boost::bad_lexical_cast& ) {
        return (test_suite*)0;
    }

    return test.release();
}

// EOF
