//  (C) Copyright Gennadiy Rozental 2001-2002.
//  (C) Copyright Beman Dawes and Ullrich Koethe 1995-2001.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: execution_monitor.cpp,v $
//
//  Version     : $Id: execution_monitor.cpp,v 1.21 2003/02/17 10:04:21 rogeeff Exp $
//
//  Description : provides execution monitor implementation for all supported 
//  configurations, including Microsoft structured exception based, unix signals
//  based and special workarounds for borland
//
//  Note that when testing requirements or user wishes preclude use of this
//  file as a separate compilation uses, it may be #included as a header file.
//
//  Header dependencies are deliberately restricted to reduce coupling to other
//  boost libraries.
// ***************************************************************************

// Boost.Test
#include <boost/test/execution_monitor.hpp>

// BOOST
#include <boost/cstdlib.hpp>  // for exit codes
#include <boost/config.hpp>   // for workarounds

// STL
#include <string>             // for string
#include <new>                // for bad_alloc
#include <typeinfo>           // for bad_cast, bad_typeid
#include <exception>          // for exception, bad_exception
#include <stdexcept>          // for std exception hierarchy
#include <cstring>            // for C string API
#include <cassert>            // for assert

#ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::strlen; using ::strncat; }
#endif

// Microsoft + other compatible compilers such as Intel
#if defined(_MSC_VER) || (defined(__INTEL__) && defined(__MWERKS__) && __MWERKS__ >= 0x3000)

#define BOOST_MS_STRCTURED_EXCEPTION_HANDLING
#include <wtypes.h>
#include <winbase.h>
#include <excpt.h>
#include <eh.h> 

#if !defined(NDEBUG) && !defined(__MWERKS__)  // __MWERKS__ does not seem to supply implementation of C runtime debug hooks, causing linking errors
#define BOOST_MS_CRT_DEBUG_HOOK
#include <crtdbg.h>
#endif

#elif (defined(__BORLANDC__) && defined(_Windows))
#define BOOST_MS_STRCTURED_EXCEPTION_HANDLING
#include <windows.h>  // Borland 5.5.1 has its own way of doing things.

#elif defined(BOOST_HAS_SIGACTION)

#define BOOST_SIGACTION_BASED_SIGNAL_HANDLING
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#else
 
#define BOOST_NO_SIGNAL_HANDLING

#endif

namespace boost {

namespace detail {

using unit_test_framework::c_string_literal;

//  boost::execution_monitor::execute() calls boost::detail::catch_signals() to
//    execute user function with signals control
//  boost::execution_monitor::execute() calls boost::detail::report_error(...) to
//    report any caught exception and throw execution_exception

const std::size_t REPORT_ERROR_BUFFER_SIZE = 512;

static int  catch_signals( execution_monitor & exmon, bool catch_system_errors, int timeout ); //  timeout is in seconds. 0 implies none.

static void report_error( execution_exception::error_code   ec,
                          c_string_literal                  msg1,           // first part of the message
                          c_string_literal                  msg2 = "" );    // second part of the message; sum length msg1 + msg2 should
                                                                            // exceed REPORT_ERROR_BUFFER_SIZE; never concatinate mesages
                                                                            // manually, cause it should work even in case of memory lack

//____________________________________________________________________________//

// Declaration for Microsoft structured exception handling (unix alternative - signal)
#ifdef BOOST_MS_STRCTURED_EXCEPTION_HANDLING

//  this class defined per the Microsoft structured exception documentation
class ms_se_exception {
public:
    // Constructor
    explicit        ms_se_exception( unsigned int n ) 
    : m_se_id( n )                      {}

    // Destructor
                    ~ms_se_exception()  {}

    // access methods
    unsigned int    id() const          { return m_se_id; }

private:
    // Data members
    unsigned int    m_se_id;
};

//____________________________________________________________________________//

void ms_se_trans_func( unsigned int id, _EXCEPTION_POINTERS* exps );
void ms_se_forward_func( unsigned int id, _EXCEPTION_POINTERS* exps );
static void report_ms_se_error( unsigned int id );

//____________________________________________________________________________//

// Declarations for unix-style signal handling
#elif defined(BOOST_SIGACTION_BASED_SIGNAL_HANDLING)

class unix_signal_exception {
public:
    // Constructor
    unix_signal_exception( execution_exception::error_code ec, c_string_literal em )
    : m_error_code( ec ), m_error_message( em )             {}

    // Destructor
    ~unix_signal_exception()                                {}

    // access methods
    execution_exception::error_code error_code() const      { return m_error_code;    }
    c_string_literal        error_message() const   { return m_error_message; }
private:
    // Data members
    execution_exception::error_code m_error_code;
    c_string_literal        m_error_message;
};

#endif

//____________________________________________________________________________//

#if defined(BOOST_MS_CRT_DEBUG_HOOK)

int
assert_reporting_function( int reportType, char* userMessage, int* retVal )
{
    switch( reportType ) {
    case _CRT_ASSERT:
        detail::report_error( execution_exception::user_error, userMessage );
        
        return 1; // return value and retVal are not important since we never reach this line
    case _CRT_ERROR:
        detail::report_error( execution_exception::system_error, userMessage );
        
        return 1; // return value and retVal are not important since we never reach this line
    default:
        return 0; // use usual reporting method
    }
}

#endif

}  // namespace detail

// ************************************************************************** //
// **************               execution_monitor              ************** //
// ************************************************************************** //

int execution_monitor::execute( bool catch_system_errors, int timeout )
{
    using unit_test_framework::c_string_literal;

#if defined(BOOST_MS_STRCTURED_EXCEPTION_HANDLING) && !defined(__BORLANDC__)
    if( catch_system_errors )
        _set_se_translator( detail::ms_se_trans_func );
    else
        _set_se_translator( detail::ms_se_forward_func );
#endif

#if defined(BOOST_MS_CRT_DEBUG_HOOK)
    if( catch_system_errors )
        _CrtSetReportHook( &detail::assert_reporting_function );
#endif

    try {
        return detail::catch_signals( *this, catch_system_errors, timeout );
    }

    //  Catch-clause reference arguments are a bit different from function
    //  arguments (ISO 15.3 paragraphs 18 & 19).  Apparently const isn't
    //  required.  Programmers ask for const anyhow, so we supply it.  That's
    //  easier than answering questions about non-const usage.

    catch( c_string_literal ex )
      { detail::report_error( execution_exception::cpp_exception_error, "C string: ", ex ); }
    catch( std::string const& ex )
    { detail::report_error( execution_exception::cpp_exception_error, "std::string: ", ex.c_str() ); }

    //  std:: exceptions

    catch( std::bad_alloc const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_alloc: ", ex.what() ); }

#if !defined(__BORLANDC__) || __BORLANDC__ > 0x0551
    catch( std::bad_cast const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_cast: ", ex.what() ); }
    catch( std::bad_typeid const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_typeid: ", ex.what() ); }
#else
    catch( std::bad_cast const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_cast" ); }
    catch( std::bad_typeid const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_typeid" ); }
#endif

    catch( std::bad_exception const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::bad_exception: ", ex.what() ); }
    catch( std::domain_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::domain_error: ", ex.what() ); }
    catch( std::invalid_argument const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::invalid_argument: ", ex.what() ); }
    catch( std::length_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::length_error: ", ex.what() ); }
    catch( std::out_of_range const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::out_of_range: ", ex.what() ); }
    catch( std::range_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::range_error: ", ex.what() ); }
    catch( std::overflow_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::overflow_error: ", ex.what() ); }
    catch( std::underflow_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::underflow_error: ", ex.what() ); }
    catch( std::logic_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::logic_error: ", ex.what() ); }
    catch( std::runtime_error const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::runtime_error: ", ex.what() ); }
    catch( std::exception const& ex )
      { detail::report_error( execution_exception::cpp_exception_error, "std::exception: ", ex.what() ); }

#if   defined(BOOST_MS_STRCTURED_EXCEPTION_HANDLING)
    catch( detail::ms_se_exception const& ex )
      { detail::report_ms_se_error( ex.id() ); }
#elif defined(BOOST_SIGACTION_BASED_SIGNAL_HANDLING)
    catch( detail::unix_signal_exception const& ex )
      { detail::report_error( ex.error_code(), ex.error_message() ); }
#endif  // BOOST_SIGACTION_BASED_SIGNAL_HANDLING

    catch( execution_exception const& ) { throw; }

    catch( ... )
      { detail::report_error( execution_exception::cpp_exception_error, "unknown type" ); }

    return 0;  // never reached; supplied to quiet compiler warnings
} // execute

//____________________________________________________________________________//

namespace detail {

#if defined(BOOST_SIGACTION_BASED_SIGNAL_HANDLING)

// ************************************************************************** //
// **************          boost::detail::signal_handler       ************** //
// ************************************************************************** //

class signal_handler {
public:
    // Constructor
    explicit signal_handler( bool catch_system_errors, int timeout );

    // Destructor
    ~signal_handler();

    // access methods
    static sigjmp_buf&      jump_buffer()
    {
        assert( s_active_handler );

        return s_active_handler->m_sigjmp_buf;
    }

private:
    // Data members
    struct sigaction        m_same_action_for_all_signals;
    struct sigaction        m_old_SIGFPE_action;
    struct sigaction        m_old_SIGTRAP_action;
    struct sigaction        m_old_SIGSEGV_action;
    struct sigaction        m_old_SIGBUS_action;
    struct sigaction        m_old_SIGABRT_action;
    struct sigaction        m_old_SIGALRM_action;

    sigjmp_buf              m_sigjmp_buf;

    signal_handler*         m_prev_handler;
    static signal_handler*  s_active_handler;

    bool                    m_catch_system_errors;
    bool                    m_set_timeout;
};

signal_handler* signal_handler::s_active_handler = NULL; // need to be placed in thread specific storage

//____________________________________________________________________________//

extern "C" {

static void execution_monitor_signal_handler( int sig )
{
    siglongjmp( signal_handler::jump_buffer(), sig );
}

}

//____________________________________________________________________________//

signal_handler::signal_handler( bool catch_system_errors, int timeout )
: m_prev_handler( s_active_handler ),
  m_catch_system_errors( catch_system_errors ),
  m_set_timeout( timeout > 0 )
{
    s_active_handler = this;
    
    if( m_catch_system_errors || m_set_timeout ) {
        m_same_action_for_all_signals.sa_flags   = 0;
        m_same_action_for_all_signals.sa_handler = &execution_monitor_signal_handler;
        sigemptyset( &m_same_action_for_all_signals.sa_mask );
    }
    
    if( m_catch_system_errors ) {
        sigaction( SIGFPE , &m_same_action_for_all_signals, &m_old_SIGFPE_action  );
        sigaction( SIGTRAP, &m_same_action_for_all_signals, &m_old_SIGTRAP_action );
        sigaction( SIGSEGV, &m_same_action_for_all_signals, &m_old_SIGSEGV_action );
        sigaction( SIGBUS , &m_same_action_for_all_signals, &m_old_SIGBUS_action  );
        sigaction( SIGABRT, &m_same_action_for_all_signals, &m_old_SIGABRT_action  );
    }
    
    if( m_set_timeout ) {
        sigaction( SIGALRM , &m_same_action_for_all_signals, &m_old_SIGALRM_action );
        alarm( timeout );
    }
}

//____________________________________________________________________________//

signal_handler::~signal_handler()
{
    typedef struct sigaction* sigaction_ptr;

    assert( s_active_handler == this );

    if( m_set_timeout ) {
        alarm( 0 );
        sigaction( SIGALRM, &m_old_SIGALRM_action, sigaction_ptr() );
    }

    if( m_catch_system_errors ) {
        sigaction( SIGFPE , &m_old_SIGFPE_action , sigaction_ptr() );
        sigaction( SIGTRAP, &m_old_SIGTRAP_action, sigaction_ptr() );
        sigaction( SIGSEGV, &m_old_SIGSEGV_action, sigaction_ptr() );
        sigaction( SIGBUS , &m_old_SIGBUS_action , sigaction_ptr() );
        sigaction( SIGABRT, &m_old_SIGABRT_action, sigaction_ptr() );
    }

    s_active_handler = m_prev_handler;
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************          boost::detail::catch_signals        ************** //
// ************************************************************************** //

int catch_signals( execution_monitor & exmon, bool catch_system_errors, int timeout )
{
    signal_handler                  local_signal_handler( catch_system_errors, timeout );
    int                             result = 0;
    execution_exception::error_code ec     = execution_exception::no_error;
    c_string_literal                em     = c_string_literal();

    volatile int sigtype = sigsetjmp( signal_handler::jump_buffer(), 1 );
    if( sigtype == 0 ) {
        result = exmon.function();
    }
    else {
        switch(sigtype) {
        case SIGALRM:
            ec = execution_exception::timeout_error;
            em = "signal: SIGALRM (timeout while executing function)";
            break;
        case SIGTRAP:
            ec = execution_exception::system_error;
            em = "signal: SIGTRAP (perhaps integer divide by zero)";
            break;
        case SIGFPE:
            ec = execution_exception::system_error;
            em = "signal: SIGFPE (arithmetic exception)";
            break;
        case SIGABRT:
            ec = execution_exception::system_error;
            em = "signal: SIGABRT (application abort requested)";
            break;
        case SIGSEGV:
        case SIGBUS:
            ec = execution_exception::system_fatal_error;
            em = "signal: memory access violation";
            break;
        default:
            ec = execution_exception::system_error;
            em = "signal: unrecognized signal";
        }
    }

    if( ec != execution_exception::no_error ) {
        throw unix_signal_exception( ec, em );
    }

    return result;
}  // unix catch_signals

//____________________________________________________________________________//

#elif (defined(__BORLANDC__) && defined(_Windows))

// this works for Borland but not other Win32 compilers (which trap too many cases)
int catch_signals( execution_monitor & exmon, bool catch_system_errors, int )
{
    int result;

    if( catch_system_errors ) {
        __try { result = exmon.function(); }
        
        __except (1)
        {
            throw ms_se_exception( GetExceptionCode() );
        }
    }
    else {
        result = exmon.function();
    }
    return result;
}

#else  // default signal handler

int catch_signals( execution_monitor & exmon, bool, int )
{
    return exmon.function();
}

#endif  // choose signal handler

// ************************************************************************** //
// **************   Microsoft structured exception handling    ************** //
// ************************************************************************** //

#if defined(BOOST_MS_STRCTURED_EXCEPTION_HANDLING)

void
ms_se_trans_func( unsigned int id, _EXCEPTION_POINTERS* /* exps */ )
{
    throw ms_se_exception( id );
}

//____________________________________________________________________________//

void
ms_se_forward_func( unsigned int /* id */, _EXCEPTION_POINTERS* /* exps */ )
{
    throw;
}

//____________________________________________________________________________//

void
report_ms_se_error( unsigned int id )
{
    switch( id ) {
        // cases classified as fatal_system_error
    case EXCEPTION_ACCESS_VIOLATION:
        detail::report_error( execution_exception::system_fatal_error, "memory access violation" );
        break;

    case EXCEPTION_ILLEGAL_INSTRUCTION:
        detail::report_error( execution_exception::system_fatal_error, "illegal instruction" );
        break;

    case EXCEPTION_PRIV_INSTRUCTION:
        detail::report_error( execution_exception::system_fatal_error, "privilaged instruction" );
        break;

    case EXCEPTION_IN_PAGE_ERROR:
        detail::report_error( execution_exception::system_fatal_error, "memory page error" );
        break;

    case EXCEPTION_STACK_OVERFLOW:
        detail::report_error( execution_exception::system_fatal_error, "stack overflow" );
        break;

        // cases classified as (non-fatal) system_trap
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        detail::report_error( execution_exception::system_error, "data misalignment" );
        break;

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        detail::report_error( execution_exception::system_error, "integer divide by zero" );
        break;

    case EXCEPTION_INT_OVERFLOW:
        detail::report_error( execution_exception::system_error, "integer overflow" );
        break;

    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        detail::report_error( execution_exception::system_error, "array bounds exceeded" );
        break;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        detail::report_error( execution_exception::system_error, "floating point divide by zero" );
        break;

    case EXCEPTION_FLT_STACK_CHECK:
        detail::report_error( execution_exception::system_error, "floating point stack check" );
        break;

    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_UNDERFLOW:
        detail::report_error( execution_exception::system_error, "floating point error" );
        break;

    default:
        detail::report_error( execution_exception::system_error, "unrecognized exception or signal" );
    }  // switch
}  // report_ms_se_error

//____________________________________________________________________________//

#endif  // Microsoft structured exception handling

// ************************************************************************** //
// **************                  report_error                ************** //
// ************************************************************************** //

static void report_error( execution_exception::error_code ec, c_string_literal msg1, c_string_literal msg2 )
{
    static char buf[REPORT_ERROR_BUFFER_SIZE];
    buf[0] = '\0';
    std::strncat( buf, msg1, sizeof(buf)-1 );
    std::strncat( buf, msg2, sizeof(buf)-1-std::strlen(buf) );
    throw execution_exception( ec, buf );
}

//____________________________________________________________________________//

} // namespace detail

} // namespace boost

// ***************************************************************************
//  Revision History :
//  
//  $Log: execution_monitor.cpp,v $
//  Revision 1.21  2003/02/17 10:04:21  rogeeff
//  some exception safety and reentrance issues addressed
//
//  Revision 1.20  2003/02/15 21:56:14  rogeeff
//  temporary cwpro fix for most of link problems
//
//  Revision 1.19  2003/02/14 06:40:00  rogeeff
//  use BOOST_HAS_SIGACTION for signal based algorithm selection
//
//  Revision 1.18  2003/02/13 08:34:12  rogeeff
//  tentative fix for signal handling selection algorithm
//  other minor fixes
//
//  Revision 1.17  2002/12/18 17:04:18  beman_dawes
//  quiet unused parameter warning
//
//  Revision 1.16  2002/12/17 22:01:09  beman_dawes
//  Add :space to std::string to prevent garbled msg
//
//  Revision 1.15  2002/12/10 21:09:49  beman_dawes
//  Metrowerks fix
//
//  Revision 1.14  2002/12/08 18:02:14  rogeeff
//  MS C runtime debug hooks added
//  switched to csignal and csetjmp
//  switched to use c_string_literal
//  catch_system_errors switch introduced for all configurations
//  SIGABRT catch added
//  REPORT_ERROR_BUFFER_SIZE is named
//
//  Revision 1.13  2002/11/02 20:04:41  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
