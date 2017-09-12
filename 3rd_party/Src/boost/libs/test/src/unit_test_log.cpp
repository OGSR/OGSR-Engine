//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: unit_test_log.cpp,v $
//
//  Version     : $Id: unit_test_log.cpp,v 1.12 2003/02/15 22:25:42 rogeeff Exp $
//
//  Description : implemets Unit Test Log, Majority of implementation details
//  are hidden in this file with use of pimpl idiom.
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/progress.hpp>
#include <boost/version.hpp>

// STL
#include <iostream>
#include <iomanip>
#include <cstring>

# ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using :: strcmp; }
# endif

namespace boost {

namespace unit_test_framework {

// ************************************************************************** //
// **************            unit_test_log_formatter           ************** //
// ************************************************************************** //

class unit_test_log_formatter {
public:
    enum log_entry_types { BOOST_UTL_ET_INFO, 
                           BOOST_UTL_ET_MESSAGE,
                           BOOST_UTL_ET_WARNING,
                           BOOST_UTL_ET_ERROR,
                           BOOST_UTL_ET_FATAL_ERROR };

    // Constructor
    explicit unit_test_log_formatter( unit_test_log const& log )
    : m_log_impl( log.m_pimpl ) {}

    // Destructor
    virtual             ~unit_test_log_formatter() {}

    // formatter interface
    virtual void        start_log( std::ostream& output, bool log_build_info ) = 0;
    virtual void        log_header( std::ostream& output, unit_test_counter test_cases_amount ) = 0;
    virtual void        finish_log( std::ostream& output ) = 0;

    virtual void        track_test_case_scope( std::ostream& output, test_case const& tc, bool in_out ) = 0;
    virtual void        log_exception( std::ostream& output, std::string const& test_case_name, c_string_literal explanation ) = 0;

    // different value logging functions
    virtual void        begin_log_entry( std::ostream& output, log_entry_types let ) = 0;
    virtual void        log_entry_value( std::ostream& output, std::string const& value ) = 0;
    virtual void        end_log_entry( std::ostream& output ) = 0;

protected:
    unit_test_log::Impl const* m_log_impl;
};

class msvc65_like_log_formatter;

// ************************************************************************** //
// **************                 unit_test_log                ************** //
// ************************************************************************** //

struct unit_test_log::Impl {
    // Constructor
    Impl() : m_stream( &std::cout ) {}

    // log data
    std::ostream*       m_stream;
    log_level           m_threshold_level;
    boost::scoped_ptr<boost::progress_display> m_progress_display;
    boost::scoped_ptr<unit_test_log_formatter> m_log_formatter;

    // entry data
    log_level           m_entry_level;
    bool                m_entry_in_progress;
    bool                m_entry_has_value;
    c_string_literal    m_entry_file;
    std::size_t         m_entry_line;

    // checkpoint data
    c_string_literal    m_checkpoint_file;
    std::size_t         m_checkpoint_line;
    std::string         m_checkpoint_message;

    // helper functions
    std::ostream&       stream()            { return *m_stream; }
    void                flush_entry()       { stream() << std::endl; }
    void                clear_entry_data()
    {
        m_entry_level           = log_nothing;
        m_entry_in_progress     = false;
        m_entry_has_value       = false;
        m_entry_file            = c_string_literal();
        m_entry_line            = 0;
    }
    void                set_checkpoint( checkpoint const& cp )
    {
        m_checkpoint_message   = cp.m_message;
        m_checkpoint_file      = m_entry_file;
        m_checkpoint_line      = m_entry_line;
    }
    void                clear_checkpoint()
    {
        m_checkpoint_file       = c_string_literal();
        m_checkpoint_line       = 0;
        m_checkpoint_message    = "";
    }

};

//____________________________________________________________________________//

unit_test_log&
unit_test_log::instance()
{
    static unit_test_log the_instance;

    return the_instance;
}

//____________________________________________________________________________//

unit_test_log::~unit_test_log()
{
    delete m_pimpl;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_stream( std::ostream& str )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    m_pimpl->m_stream = &str;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_threshold_level( log_level lev )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    m_pimpl->m_threshold_level = lev;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_threshold_level_by_name( std::string const& lev )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    struct my_pair {
        c_string_literal    level_name;
        log_level           level_value;
    };

    static const my_pair name_value_map[] = {
        { "all"             , log_successful_tests },
        { "success"         , log_successful_tests },
        { "test_suite"      , log_test_suites },
        { "messages"        , log_messages },
        { "warnings"        , log_warnings },
        { "all_errors"      , log_all_errors },
        { "cpp_exceptions"  , log_cpp_exception_errors },
        { "system_errors"   , log_system_errors },
        { "fatal_errors"    , log_fatal_errors },
        { "progress"        , log_progress_only},
        { "nothing"         , log_nothing },
    };

    static int const map_size = sizeof(name_value_map)/sizeof(my_pair);

    if( lev.empty() )
        return;

    for( int i=0; i < map_size; i++ ) {
        if( lev == name_value_map[i].level_name ) {
            set_log_threshold_level( name_value_map[i].level_value );
            return;
        }
    }
}

//____________________________________________________________________________//

void
unit_test_log::clear_checkpoint()
{
    m_pimpl->clear_checkpoint();
}

//____________________________________________________________________________//

void
unit_test_log::track_test_case_scope( test_case const& tc, bool in_out )
{
    if( m_pimpl->m_threshold_level > log_test_suites )
        return;

    *this << begin();

    m_pimpl->m_log_formatter->track_test_case_scope( m_pimpl->stream(), tc, in_out );
    m_pimpl->m_entry_has_value = true;

    *this << end();
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( begin const& )
{
    if( m_pimpl->m_entry_in_progress )
        *this << end();

    m_pimpl->m_entry_in_progress = true;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( end const& )
{
    if( m_pimpl->m_entry_has_value ) {
        m_pimpl->m_log_formatter->end_log_entry( m_pimpl->stream() );
        m_pimpl->flush_entry();
    }

    m_pimpl->clear_entry_data();

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( file const& f )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->m_entry_file = f.m_file_name;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( line const& l )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->m_entry_line = l.m_line_num;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( level const& lev )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->m_entry_level = lev.m_level;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( checkpoint const& cp )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->set_checkpoint( cp );

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( log_exception const& re )
{
    if( m_pimpl->m_entry_in_progress && m_pimpl->m_entry_level >= m_pimpl->m_threshold_level ) {
        m_pimpl->m_log_formatter->log_exception( m_pimpl->stream(), unit_test_result::instance().test_case_name(), re.m_what );
        m_pimpl->m_entry_has_value = true;
    }

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( log_progress const& )
{
    if( m_pimpl->m_progress_display )
        ++(*m_pimpl->m_progress_display);

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( std::string const& value )
{
    if( m_pimpl->m_entry_in_progress && m_pimpl->m_entry_level >= m_pimpl->m_threshold_level && !value.empty() ) {
        if( !m_pimpl->m_entry_has_value ) {
            switch( m_pimpl->m_entry_level ) {
            case log_successful_tests:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_INFO );
                break;
            case log_messages:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_MESSAGE );
                break;
            case log_warnings:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_WARNING );
                break;
            case log_all_errors:
            case log_cpp_exception_errors:
            case log_system_errors:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_ERROR );
                break;
            case log_fatal_errors:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_FATAL_ERROR );
                break;
            case log_progress_only:
            case log_nothing:
            case log_test_suites:
                return *this;
            }
        }

        m_pimpl->m_log_formatter->log_entry_value( m_pimpl->stream(), value );
        m_pimpl->m_entry_has_value = true;
    }

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( c_string_literal value )
{
    return *this << std::string( value ? value : "" );
}

//____________________________________________________________________________//

void
unit_test_log::start( bool print_build_info )
{
    m_pimpl->m_log_formatter->start_log( m_pimpl->stream(), print_build_info );
}

//____________________________________________________________________________//

void
unit_test_log::header( unit_test_counter test_cases_amount )
{
    if( m_pimpl->m_threshold_level != log_nothing && test_cases_amount > 0 )
        m_pimpl->m_log_formatter->log_header( m_pimpl->stream(), test_cases_amount );

    if( m_pimpl->m_threshold_level == log_progress_only )
        m_pimpl->m_progress_display.reset(
            new boost::progress_display( test_cases_amount, m_pimpl->stream() ) );
    else
        m_pimpl->m_progress_display.reset();
}

//____________________________________________________________________________//

void
unit_test_log::finish( unit_test_counter test_cases_amount )
{
    if( test_cases_amount == 1 )
        *this << log_progress();

    m_pimpl->m_log_formatter->finish_log( m_pimpl->stream() );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************           msvc65_like_log_formatter          ************** //
// ************************************************************************** //

class msvc65_like_log_formatter : public unit_test_log_formatter {
public:
    explicit msvc65_like_log_formatter( unit_test_log const& log ) 
    : unit_test_log_formatter( log ) {}

    void    start_log( std::ostream& output, bool log_build_info )
    {
        if( log_build_info )
            output  << "Platform: " << BOOST_PLATFORM            << '\n'
                    << "Compiler: " << BOOST_COMPILER            << '\n'
                    << "STL     : " << BOOST_STDLIB              << '\n'
                    << "Boost   : " << BOOST_VERSION/100000      << "."
                                    << BOOST_VERSION/100 % 1000  << "."
                                    << BOOST_VERSION % 100       << '\n';
    }
    void    log_header( std::ostream& output, unit_test_counter test_cases_amount )
    {
        output  << "Running " << test_cases_amount << " test "
                << (test_cases_amount > 1 ? "cases" : "case") << "...\n";
    }

    void    finish_log( std::ostream& output ) {}

    void    track_test_case_scope( std::ostream& output, test_case const& tc, bool in_out )
    {
        output << (in_out ? "Entering" : "Leaving")
               << " test " << ( tc.p_type ? "case" : "suite" )
               << " \"" << tc.p_name.get() << "\"";
    }

    void    log_exception( std::ostream& output, std::string const& test_case_name, c_string_literal explanation )
    {
        output << "Exception in \"" << test_case_name << "\": " << explanation;

        if( !m_log_impl->m_checkpoint_message.empty() ) {
            output << '\n' << m_log_impl->m_checkpoint_file << '(' << m_log_impl->m_checkpoint_line << ") : "
                << "last checkpoint: " << m_log_impl->m_checkpoint_message;
        }
    }

    void    begin_log_entry( std::ostream& output, log_entry_types let )
    {
        switch( let ) {
        case BOOST_UTL_ET_INFO:
            print_prefix( output );
            output << "info: ";
            break;
        case BOOST_UTL_ET_MESSAGE:
            break;
        case BOOST_UTL_ET_WARNING:
            print_prefix( output );
            output << "warning in \"" << unit_test_result::instance().test_case_name() << "\": ";
            break;
        case BOOST_UTL_ET_ERROR:
            print_prefix( output );
            output << "error in \"" << unit_test_result::instance().test_case_name() << "\": ";
            break;
        case BOOST_UTL_ET_FATAL_ERROR:
            print_prefix( output );
            output << "fatal error in \"" << unit_test_result::instance().test_case_name() << "\": ";
            break;
        }
    }

    void    log_entry_value( std::ostream& output, std::string const& value )
    {
        output << value;
    }

    void    end_log_entry( std::ostream& output ) {}

protected:
    virtual void    print_prefix( std::ostream& output )
    {
        output << m_log_impl->m_entry_file << '(' << m_log_impl->m_entry_line << "): ";
    }

};

// ************************************************************************** //
// **************               xml_log_formatter              ************** //
// ************************************************************************** //

class xml_log_formatter : public unit_test_log_formatter {
public:
    // Constructor
    explicit xml_log_formatter( unit_test_log const& log ) 
    : unit_test_log_formatter( log ), m_indent( 0 ), m_curr_tag( c_string_literal() ) {}

    void    start_log( std::ostream& output, bool log_build_info )
    {
        output  << "<TestLog";

        if( log_build_info )
            output  << " platform=\"" << BOOST_PLATFORM            << '\"'
                    << " compiler=\"" << BOOST_COMPILER            << '\"'
                    << " stl=\""      << BOOST_STDLIB              << '\"'
                    << " boost=\""    << BOOST_VERSION/100000      << "."
                                      << BOOST_VERSION/100 % 1000  << "."
                                      << BOOST_VERSION % 100       << '\"';

        output  << ">\n";
    }
    void    log_header( std::ostream& output, unit_test_counter test_cases_amount ) {}
    void    finish_log( std::ostream& output )
    {
        output  << "</TestLog>\n";
    }

    void    track_test_case_scope( std::ostream& output, test_case const& tc, bool in_out )
    {
        if( !in_out )
            m_indent -= 2;

        print_indent( output );

        output << (in_out ? "<" : "</")
               << ( tc.p_type ? "TestCase" : "TestSuite" );

        if( in_out )
            output << " name=\"" << tc.p_name.get() << "\"";

        output << ">";

        if( in_out )
            m_indent += 2;
    }

    void    log_exception( std::ostream& output, std::string const& test_case_name, c_string_literal explanation )
    {
        print_indent( output );

        output << "<Exception name=\"" << test_case_name << "\">\n";

        m_indent += 2;
        print_indent( output );

        output << explanation << '\n';
        print_indent( output );

        if( !m_log_impl->m_checkpoint_message.empty() ) {
            output << "<LastCheckpoint file=\"" << m_log_impl->m_checkpoint_file << "\""
                   << " line=\"" << m_log_impl->m_checkpoint_line << "\">\n";

            m_indent += 2;
            print_indent( output );

            output << m_log_impl->m_checkpoint_message << "\n";

            m_indent -= 2;
            print_indent( output );

            output << "</LastCheckpoint>\n";

            m_indent -= 2;
            print_indent( output );
        }

        output << "</Exception>";
    }

    void    begin_log_entry( std::ostream& output, log_entry_types let )
    {
        static c_string_literal const xml_tags[] = { "Info", "Message", "Warning", "Error", "FatalError" };

        print_indent( output );
        
        m_curr_tag = xml_tags[let];
        output << '<' << m_curr_tag
               << " file=\"" << m_log_impl->m_entry_file << '\"'
               << " line=\"" << m_log_impl->m_entry_line << '\"'
               << ">\n";

        m_indent += 2;
        print_indent( output );
    }

    void    log_entry_value( std::ostream& output, std::string const& value )
    {
        output << value;
    }

    void    end_log_entry( std::ostream& output )
    {
        if( !m_curr_tag )
            return;

        output << '\n';

        m_indent -= 2;
        print_indent( output );

        output << "</" << m_curr_tag << ">";

        m_curr_tag = c_string_literal();
    }

private:
    void    print_indent( std::ostream& output )
    {
        output << std::setw( m_indent ) << "";
    }

    // Data members
    std::size_t      m_indent;
    c_string_literal m_curr_tag;
};

// ************************************************************************** //
// **************                 unit_test_log                ************** //
// ************************************************************************** //

unit_test_log::unit_test_log() : m_pimpl( new Impl() )
{
    m_pimpl->m_threshold_level = log_all_errors;

    m_pimpl->m_log_formatter.reset( new msvc65_like_log_formatter( *this ) );

    m_pimpl->clear_entry_data();
    m_pimpl->clear_checkpoint();
}

//____________________________________________________________________________//

void
unit_test_log::set_log_format( std::string const& logformat )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    struct my_pair {
        c_string_literal    format_name;
        output_format       format_value;
    };

    static const my_pair name_value_map[] = {
        { "HRF" , HRF },
        { "XML" , XML },
    };

    static int const map_size = sizeof(name_value_map)/sizeof(my_pair);

    output_format of = HRF;
    for( int i=0; i < map_size; i++ ) {
        if( logformat == name_value_map[i].format_name ) {
            of = name_value_map[i].format_value;
            break;
        }
    }

    if( of == HRF )
        m_pimpl->m_log_formatter.reset( new msvc65_like_log_formatter( *this ) );
    else
        m_pimpl->m_log_formatter.reset( new xml_log_formatter( *this ) );
}

//____________________________________________________________________________//

} // namespace unit_test_framework

} // namespace boost

// ***************************************************************************
//  Revision History :
//
//  $Log: unit_test_log.cpp,v $
//  Revision 1.12  2003/02/15 22:25:42  rogeeff
//  include parameters moved to source file
//
//  Revision 1.11  2003/02/13 08:36:54  rogeeff
//  reworked to support multiply log formats
//
//  Revision 1.10  2002/12/08 18:07:16  rogeeff
//  switched to use c_string_literal
//  all NULLs substituted with c_string_literal default constructors
//  build info is printed independently of report level
//
//  Revision 1.9  2002/11/02 20:04:42  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
