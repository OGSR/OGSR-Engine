//  (C) Copyright Gennadiy Rozental 2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: prg_exec_fail3.cpp,v $
//
//  Version     : $Id: prg_exec_fail3.cpp,v 1.1 2002/12/09 05:13:31 rogeeff Exp $
//
//  Description : tests an ability of Program Execution Monitor to catch 
//  user fatal exceptions. Should fail during run. But not crash.
// ***************************************************************************

#include <cassert>

int cpp_main( int, char *[] )  // note the name
{
  int div = 0;

  assert( div != 0 );

  return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: prg_exec_fail3.cpp,v $
//  Revision 1.1  2002/12/09 05:13:31  rogeeff
//  Initial commit
//

// ***************************************************************************

// EOF
