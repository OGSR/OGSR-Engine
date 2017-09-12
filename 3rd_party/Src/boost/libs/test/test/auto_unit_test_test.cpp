//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: auto_unit_test_test.cpp,v $
//
//  Version     : $Id: auto_unit_test_test.cpp,v 1.1 2002/12/09 05:13:31 rogeeff Exp $
//
//  Description : unit test for auto unit test facility
// ***************************************************************************

// Boost.Test
#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_UNIT_TEST( test1 )
{
    BOOST_CHECK( true );
}

BOOST_AUTO_UNIT_TEST( test2 )
{
    BOOST_CHECK( true );
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: auto_unit_test_test.cpp,v $
//  Revision 1.1  2002/12/09 05:13:31  rogeeff
//  Initial commit
//

// ***************************************************************************

// EOF
