//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes 2001. 
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: prg_exec_fail2.cpp,v $
//
//  Version     : $Id: prg_exec_fail2.cpp,v 1.6 2002/12/09 05:16:50 rogeeff Exp $
//
//  Description : tests an ability of Program Execution Monitor to catch 
//  system exceptions. Should fail during run.
// ***************************************************************************

int cpp_main( int, char *[] )  // note the name
{
    int div = 0;
    return 10 / div;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: prg_exec_fail2.cpp,v $
//  Revision 1.6  2002/12/09 05:16:50  rogeeff
//  *** empty log message ***
//
//  Revision 1.5  2002/11/02 20:04:43  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
