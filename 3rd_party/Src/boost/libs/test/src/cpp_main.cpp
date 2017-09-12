//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes 1995-2001.
//  Permission to copy, use, modify, sell and distribute this software is 
//  granted provided this copyright notice appears in all copies. This software
//  is provided "as is" without express or implied warranty, and with no 
//  claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: cpp_main.cpp,v $
//
//  Version     : $Id: cpp_main.cpp,v 1.7 2002/12/08 17:58:10 rogeeff Exp $
//
//  Description : main function implementation for Program Executon Monitor
// ***************************************************************************

// Boost.Test
#include <boost/test/execution_monitor.hpp>
#include <boost/test/detail/unit_test_config.hpp>

// BOOST
#include <boost/cstdlib.hpp>
#include <boost/config.hpp>

// STL
#include <iostream>

#ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::getenv; using ::strcmp; }
#endif

int cpp_main( int argc, char* argv[] );  // prototype for user's cpp_main()

namespace
{

class cpp_main_caller : public boost::execution_monitor {
public:
    cpp_main_caller( int argc, char** argv )
    : m_argc( argc ), m_argv( argv )    {}
    
    int         function()              { return cpp_main( m_argc, m_argv ); }
    
private:
    int         m_argc;
    char**      m_argv;
};

}

// ************************************************************************** //
// **************                   cpp main                   ************** //
// ************************************************************************** //

int main( int argc, char* argv[] )
{
    cpp_main_caller caller( argc, argv );
    
    int result;

    boost::unit_test_framework::c_string_literal p( std::getenv( "BOOST_TEST_CATCH_SYSTEM_ERRORS" ) );
    bool catch_system_errors = !p || (std::strcmp( p, "no" ) != 0);
        
    try {
        result = caller.execute( catch_system_errors );
        
        //  Some prefer a confirming message when all is well, while others don't
        //  like the clutter.  Use an environmental variable to avoid command
        //  line argument modifications; for use in production programs
        //  that's a no-no in some organizations.
        boost::unit_test_framework::c_string_literal p( std::getenv( "BOOST_PRG_MON_CONFIRM" ) );
        if( !p || std::strcmp( p, "no" ) != 0 ) { 
            std::cout << std::flush << "no errors detected" << std::endl; 
        }
    }
    catch( boost::execution_exception const& exex ) {
        std::cout << "\n*** Error: " << exex.what() << std::endl;
        result = exex.code();
    }
    
    if( result != 0 && result != boost::exit_success ) {
        std::cout << "\n**** error return code " << result << std::endl;
        std::cerr << "**********  errors detected; see standard output for details  ***********" << std::endl;
    }

    return result;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: cpp_main.cpp,v $
//  Revision 1.7  2002/12/08 17:58:10  rogeeff
//  BOOST_TEST_SYSTEM_ERROS env. variable introduced
//  BOOST_CPP_MAIN_CONFIRMATION renamed to BOOST_PRG_MAN_CONFIRM and twicked it's
//  logic a bit. It now should have value "no" to turn off pass confirmation
//
//  Revision 1.6  2002/11/02 20:04:41  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
