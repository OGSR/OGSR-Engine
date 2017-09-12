//  Boost GCD & LCM common_factor.hpp test program  --------------------------//

//  (C) Copyright Daryle Walker 2001.  Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies.  This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

//  See http://www.boost.org for most recent version including documentation.

//  Revision History
//  07 Nov 2001  Initial version (Daryle Walker)

#define  BOOST_INCLUDE_MAIN

#include <boost/config.hpp>              // for BOOST_MSVC
#include <boost/cstdlib.hpp>             // for boost::exit_success
#include <boost/math/common_factor.hpp>  // for boost::math::gcd, etc.
#include <boost/test/test_tools.hpp>     // for main, BOOST_TEST

#include <iostream>  // for std::cout (std::endl indirectly)


// Control to determine what kind of built-in integers are used
#ifndef CONTROL_INT_TYPE
#define CONTROL_INT_TYPE  int
#endif


// Main testing function
int
test_main
(
    int         ,   // "argc" is unused
    char *      []  // "argv" is unused
)
{    
    using std::cout;
    using std::endl;

#ifndef BOOST_MSVC
    using boost::math::gcd;
    using boost::math::static_gcd;
    using boost::math::lcm;
    using boost::math::static_lcm;
#else
    using namespace boost::math;
#endif

    typedef CONTROL_INT_TYPE  int_type;
    typedef unsigned CONTROL_INT_TYPE uint_type;

    // GCD tests
    cout << "Doing tests on gcd." << endl;

    BOOST_TEST( gcd<int_type>(  1,  -1) ==  1 );
    BOOST_TEST( gcd<int_type>( -1,   1) ==  1 );
    BOOST_TEST( gcd<int_type>(  1,   1) ==  1 );
    BOOST_TEST( gcd<int_type>( -1,  -1) ==  1 );
    BOOST_TEST( gcd<int_type>(  0,   0) ==  0 );
    BOOST_TEST( gcd<int_type>(  7,   0) ==  7 );
    BOOST_TEST( gcd<int_type>(  0,   9) ==  9 );
    BOOST_TEST( gcd<int_type>( -7,   0) ==  7 );
    BOOST_TEST( gcd<int_type>(  0,  -9) ==  9 );
    BOOST_TEST( gcd<int_type>( 42,  30) ==  6 );
    BOOST_TEST( gcd<int_type>(  6,  -9) ==  3 );
    BOOST_TEST( gcd<int_type>(-10, -10) == 10 );
    BOOST_TEST( gcd<int_type>(-25, -10) ==  5 );
    BOOST_TEST( gcd<int_type>(  3,   7) ==  1 );
    BOOST_TEST( gcd<int_type>(  8,   9) ==  1 );
    BOOST_TEST( gcd<int_type>(  7,  49) ==  7 );

    // GCD tests
    cout << "Doing tests on unsigned-gcd." << endl;

    BOOST_TEST( gcd<uint_type>(  1u,   1u) ==  1u );
    BOOST_TEST( gcd<uint_type>(  0u,   0u) ==  0u );
    BOOST_TEST( gcd<uint_type>(  7u,   0u) ==  7u );
    BOOST_TEST( gcd<uint_type>(  0u,   9u) ==  9u );
    BOOST_TEST( gcd<uint_type>( 42u,  30u) ==  6u );
    BOOST_TEST( gcd<uint_type>(  3u,   7u) ==  1u );
    BOOST_TEST( gcd<uint_type>(  8u,   9u) ==  1u );
    BOOST_TEST( gcd<uint_type>(  7u,  49u) ==  7u );

    cout << "Doing tests on static_gcd." << endl;

    BOOST_TEST( (static_gcd< 1,  1>::value) == 1 );
    BOOST_TEST( (static_gcd< 0,  0>::value) == 0 );
    BOOST_TEST( (static_gcd< 7,  0>::value) == 7 );
    BOOST_TEST( (static_gcd< 0,  9>::value) == 9 );
    BOOST_TEST( (static_gcd<42, 30>::value) == 6 );
    BOOST_TEST( (static_gcd< 3,  7>::value) == 1 );
    BOOST_TEST( (static_gcd< 8,  9>::value) == 1 );
    BOOST_TEST( (static_gcd< 7, 49>::value) == 7 );

    // LCM tests
    cout << "Doing tests on lcm." << endl;

    BOOST_TEST( lcm<int_type>(  1,  -1) ==  1 );
    BOOST_TEST( lcm<int_type>( -1,   1) ==  1 );
    BOOST_TEST( lcm<int_type>(  1,   1) ==  1 );
    BOOST_TEST( lcm<int_type>( -1,  -1) ==  1 );
    BOOST_TEST( lcm<int_type>(  0,   0) ==  0 );
    BOOST_TEST( lcm<int_type>(  6,   0) ==  0 );
    BOOST_TEST( lcm<int_type>(  0,   7) ==  0 );
    BOOST_TEST( lcm<int_type>( -5,   0) ==  0 );
    BOOST_TEST( lcm<int_type>(  0,  -4) ==  0 );
    BOOST_TEST( lcm<int_type>( 18,  30) == 90 );
    BOOST_TEST( lcm<int_type>( -6,   9) == 18 );
    BOOST_TEST( lcm<int_type>(-10, -10) == 10 );
    BOOST_TEST( lcm<int_type>( 25, -10) == 50 );
    BOOST_TEST( lcm<int_type>(  3,   7) == 21 );
    BOOST_TEST( lcm<int_type>(  8,   9) == 72 );
    BOOST_TEST( lcm<int_type>(  7,  49) == 49 );

    cout << "Doing tests on unsigned-lcm." << endl;

    BOOST_TEST( lcm<uint_type>(  1u,   1u) ==  1u );
    BOOST_TEST( lcm<uint_type>(  0u,   0u) ==  0u );
    BOOST_TEST( lcm<uint_type>(  6u,   0u) ==  0u );
    BOOST_TEST( lcm<uint_type>(  0u,   7u) ==  0u );
    BOOST_TEST( lcm<uint_type>( 18u,  30u) == 90u );
    BOOST_TEST( lcm<uint_type>(  3u,   7u) == 21u );
    BOOST_TEST( lcm<uint_type>(  8u,   9u) == 72u );
    BOOST_TEST( lcm<uint_type>(  7u,  49u) == 49u );

    cout << "Doing tests on static_lcm." << endl;

    BOOST_TEST( (static_lcm< 1,  1>::value) ==  1 );
    BOOST_TEST( (static_lcm< 0,  0>::value) ==  0 );
    BOOST_TEST( (static_lcm< 6,  0>::value) ==  0 );
    BOOST_TEST( (static_lcm< 0,  7>::value) ==  0 );
    BOOST_TEST( (static_lcm<18, 30>::value) == 90 );
    BOOST_TEST( (static_lcm< 3,  7>::value) == 21 );
    BOOST_TEST( (static_lcm< 8,  9>::value) == 72 );
    BOOST_TEST( (static_lcm< 7, 49>::value) == 49 );

    return boost::exit_success;
}
