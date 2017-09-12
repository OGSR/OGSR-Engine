//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes 2000.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: test_exec_fail2.cpp,v $
//
//  Version     : $Id: test_exec_fail2.cpp,v 1.6 2003/02/15 21:58:32 rogeeff Exp $
//
//  Description : test failures reported by differen Test Tools.
//  Should fail during run.
// ***************************************************************************

// Boost.Test
#include <boost/test/test_tools.hpp>

int test_main( int, char *[] )  // note the name
{
    int v = 1;

    if( v < 10 ) { // to eliminate unreachable return statement warning
        BOOST_CHECK( v == 2 );
        BOOST_ERROR( "sample BOOST_ERROR call" );
        BOOST_REQUIRE( 2 == v );

        throw "Opps! should never reach this point";
    }

    return 1;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: test_exec_fail2.cpp,v $
//  Revision 1.6  2003/02/15 21:58:32  rogeeff
//  borland warning fix
//
//  Revision 1.5  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
