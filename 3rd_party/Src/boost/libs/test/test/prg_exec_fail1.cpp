//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes 2001. 
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: prg_exec_fail1.cpp,v $
//
//  Version     : $Id: prg_exec_fail1.cpp,v 1.6 2003/02/15 21:49:58 rogeeff Exp $
//
//  Description : tests an ability of Program Execution Monitor to catch 
//  uncatched exceptions. Should fail during run.
// ***************************************************************************

int
cpp_main( int argc, char *[] )  // note the name
{
    if( argc > 0 ) // to prevent the unreachable return warning
        throw "Test error by throwing C-style string exception";

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: prg_exec_fail1.cpp,v $
//  Revision 1.6  2003/02/15 21:49:58  rogeeff
//  borland warning fix
//
//  Revision 1.5  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
