//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: minimal_test.cpp,v $
//
//  Version     : $Id: minimal_test.cpp,v 1.4 2003/02/15 21:51:56 rogeeff Exp $
//
//  Description : minimal testing unit test
// ***************************************************************************

// Boost.Test
#include <boost/test/minimal.hpp>

//____________________________________________________________________________//

struct bool_convertible1 {
    bool_convertible1( bool v ) : v_( v ) {}
    operator bool() { return v_; }

    bool v_;
};

//____________________________________________________________________________//

struct bool_convertible2 {
    bool_convertible2( int v ) : v_( v ) {}
    operator int() { return v_; }

    int v_;
};

//____________________________________________________________________________//

struct bool_convertible3 {
    bool_convertible3( void* v ) : v_( v ) {}

    struct Tester {};
    operator Tester*() { return (Tester*)v_; }

    void* v_;
};

//____________________________________________________________________________//

int 
test_main( int /*argc*/, char* /*argv*/[] ) 
{
    int i = 1;

    BOOST_CHECK( i == 1 );
    BOOST_CHECK( i == 2 );

    BOOST_CHECK( bool_convertible1( true  ) );
    BOOST_CHECK( bool_convertible1( false ) );

    BOOST_CHECK( bool_convertible2( 1 ) );
    BOOST_CHECK( bool_convertible2( 0 ) );

    BOOST_CHECK( bool_convertible3( (void*)1 ) );
    BOOST_CHECK( bool_convertible3( NULL ) );

    BOOST_ERROR( "Some error" );

    BOOST_REQUIRE( i == 4 );

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: minimal_test.cpp,v $
//  Revision 1.4  2003/02/15 21:51:56  rogeeff
//  borland warnings fix
//
//  Revision 1.3  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
