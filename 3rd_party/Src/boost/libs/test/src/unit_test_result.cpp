//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for updates, documentation, and revision history.
//
//  File        : $RCSfile: unit_test_result.cpp,v $
//
//  Version     : $Id: unit_test_result.cpp,v 1.15 2003/02/15 21:55:32 rogeeff Exp $
//
//  Description : implements Unit Test Result reporting facility. Note that majority of 
//  implementation is hidden in this file using pimple idiom.
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_result.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>

// BOOST
#include <boost/config.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdlib.hpp>

// STL
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cmath>

# ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::log10; using ::strncmp; }
# endif

namespace boost {

namespace unit_test_framework {

typedef unit_test_result* unit_test_result_ptr;

// ************************************************************************** //
// **************               report_formatter               ************** //
// ************************************************************************** //

class report_formatter {
public:
    // Destructor
    virtual         ~report_formatter() {}

    virtual void    start_result_report( std::ostream& where_to ) = 0;
    virtual void    finish_result_report( std::ostream& where_to ) = 0;

    virtual void    start_test_case_report( std::ostream& where_to, std::size_t indent, 
                                            std::string const& test_case_name, bool case_suite, bool failed ) = 0;

    virtual void    start_confirmation_report( std::ostream& where_to,
                                               std::string const& test_case_name, bool case_suite, bool failed,
                                               unit_test_counter num_failed, unit_test_counter num_expected ) = 0;

    virtual void    finish_test_case_report( std::ostream& where_to, std::size_t indent, 
                                             std::string const& test_case_name, bool case_suite, bool aborted ) = 0;

    virtual void    report_sub_test_cases_stat( std::ostream& where_to, std::size_t indent,
                                                unit_test_counter num_passed, unit_test_counter num_failed ) = 0;

    virtual void    report_assertions_stat( std::ostream& where_to, std::size_t indent,
                                            unit_test_counter num_passed, 
                                            unit_test_counter num_failed,
                                            unit_test_counter num_expected ) = 0;

};

//____________________________________________________________________________//

// ************************************************************************** //
// **************             hrf_report_formatter             ************** //
// ************************************************************************** //

class hrf_report_formatter : public report_formatter {
public:
    void    start_result_report( std::ostream& where_to ) {}
    void    finish_result_report( std::ostream& where_to ) {}

    void    start_test_case_report( std::ostream& where_to, std::size_t indent,
                                    std::string const& test_case_name, bool case_suite, bool failed )
    {
        where_to << "\n" << std::setw( indent ) << "" << "Test " << cs_name( case_suite ) << " " << quote( test_case_name )
                 << (failed ? " failed with:\n" : " passed with:\n");

    }

    void    start_confirmation_report( std::ostream& where_to,
                                       std::string const& test_case_name, bool case_suite, bool failed,
                                       unit_test_counter num_failed, unit_test_counter num_expected )
    {
        if( failed ) {
            if( num_failed == 0 ) {
                where_to << "\n*** errors detected in test " << cs_name( case_suite ) << " " << quote( test_case_name )
                         << "; see standard output for details\n";
                return;
            }


            where_to << "\n*** " << num_failed << ps_name( num_failed != 1, " failure" ) << " detected";
            
            if( num_expected > 0 )
                where_to << " (" << num_expected << ps_name( num_expected != 1, " failure" ) << " expected)";
            
            where_to << " in test " << cs_name( case_suite ) << " " << quote( test_case_name ) << "\n";
        }
        else
            where_to << "\n*** No errors detected\n";
    }

    void    finish_test_case_report( std::ostream& where_to, std::size_t indent, 
                                     std::string const& test_case_name, bool case_suite, bool aborted )
    {
        if( aborted )
            where_to << std::setw( indent ) << "" << "Test " << cs_name( case_suite ) << " " << quote( test_case_name )
                 << " was aborted due to uncaught exception, user assert or system error\n";

        where_to.flush();
    }

    void    report_sub_test_cases_stat( std::ostream& where_to, std::size_t indent, 
                                        unit_test_counter num_passed, unit_test_counter num_failed )
    {
        unit_test_counter total_test_cases = num_passed + num_failed;
        std::size_t       width = static_cast<std::size_t>( std::log10( (float)std::max( num_passed, num_failed ) ) ) + 1;

        where_to << std::setw( indent ) << "" << std::setw( width ) << num_passed
                 << " test " << ps_name( num_passed != 1, "case" ) << " out of " << total_test_cases << " passed\n"
                 << std::setw( indent ) << "" << std::setw( width ) << num_failed
                 << " test " << ps_name( num_failed != 1, "case" ) << " out of " << total_test_cases << " failed\n";

    }

    void    report_assertions_stat( std::ostream& where_to, std::size_t indent,
                                    unit_test_counter num_passed, unit_test_counter num_failed, unit_test_counter num_expected )
    {
        unit_test_counter total_assertions = num_passed + num_failed;
        std::size_t       width            = total_assertions > 0 
                                               ? static_cast<std::size_t>( std::log10( (float)std::max( num_passed, num_failed ) ) ) + 1
                                               : 1;
        
        where_to << std::setw( indent ) << "" << std::setw( width ) << num_passed 
                 << ps_name( num_passed != 1, " assertion" ) << " out of " << total_assertions << " passed\n"
                 << std::setw( indent ) << "" << std::setw( width ) << num_failed
                 << ps_name( num_failed != 1, " assertion" ) << " out of " << total_assertions << " failed\n";

        if( num_expected > 0 )
            where_to << std::setw( indent ) << "" << "while " << num_expected 
                     << ps_name( num_expected != 1, " failure" ) <<  " expected\n";
    }

private:
    static  std::string ps_name( bool p_s, std::string singular_form )
    { 
        return p_s ? singular_form.append( "s" ) : singular_form;
    }
    static  std::string cs_name( bool c_s )
    { 
        return c_s ? "case" : "suite";
    }
    static  std::string quote( std::string const& name )
    {
        return std::string( "\"" ).append( name ).append( "\"");
    }
};

// ************************************************************************** //
// **************             xml_report_formatter             ************** //
// ************************************************************************** //

class xml_report_formatter : public report_formatter {
public:
    void    start_result_report( std::ostream& where_to )
    {
        where_to << "<TestResult>\n";
    }
    void    finish_result_report( std::ostream& where_to )
    {
        where_to << "</TestResult>\n";
    }

    void    start_test_case_report( std::ostream& where_to, std::size_t indent, 
                                    std::string const& test_case_name, bool case_suite, bool failed )
    {
        where_to << std::setw( indent ) << ""
                 << "<" << ( case_suite ? "TestCase" : "TestSuite" ) 
                 << " name=\"" << test_case_name << '\"'
                 << " result=" << (failed ? "\"failed\"" : "\"passed\"" ) << ">\n";
    }

    void    start_confirmation_report( std::ostream& where_to,
                                               std::string const& test_case_name, bool case_suite, bool failed,
                                               unit_test_counter num_failed, unit_test_counter num_expected )
    {
        where_to << "<" << ( case_suite ? "TestCase" : "TestSuite" ) 
                 << " name=\"" << test_case_name << '\"'
                 << " result=" << (failed ? "\"failed\"" : "\"passed\"" );
        if( failed )
            where_to << " num_of_failures=" << num_failed
                     << " expected_failures=" << num_expected;

        where_to << ">\n";
    }

    void    finish_test_case_report( std::ostream& where_to, std::size_t indent, 
                                     std::string const& test_case_name, bool case_suite, bool aborted )
    {
        if( aborted ) {
            where_to << std::setw( indent+2 ) << ""
                     << "<" << "aborted" 
                     << " reason=" << "\"due to uncaught exception, user assert or system error\""
                     << "/>\n";
        }

        where_to << std::setw( indent ) << ""
                 << "</" << ( case_suite ? "TestCase" : "TestSuite" ) << ">\n";
    }

    void    report_sub_test_cases_stat( std::ostream& where_to, std::size_t indent,
                                        unit_test_counter num_passed, unit_test_counter num_failed )
    {
        where_to << std::setw( indent+2 ) << ""
                 << "<SubTestCases"
                 << " passed=\"" << num_passed << '\"'
                 << " failed=\"" << num_failed << '\"'
                 << "/>\n";
    }

    void    report_assertions_stat( std::ostream& where_to, std::size_t indent,
                                    unit_test_counter num_passed, 
                                    unit_test_counter num_failed,
                                    unit_test_counter num_expected )
    {
        where_to << std::setw( indent+2 ) << ""
                 << "<Asssertions"
                 << " passed=\"" << num_passed << '\"'
                 << " failed=\"" << num_failed << '\"'
                 << " expected_failures=\"" << num_expected << '\"'
                 << "/>\n";
    }
};

// ************************************************************************** //
// **************               unit_test_result               ************** //
// ************************************************************************** //

struct unit_test_result::Impl {
    unit_test_result_ptr            m_parent;
    std::list<unit_test_result_ptr> m_children;
    unit_test_counter               m_assertions_passed;
    unit_test_counter               m_assertions_failed;
    unit_test_counter               m_expected_failures;
    unit_test_counter               m_test_cases_passed;
    unit_test_counter               m_test_cases_failed;
    bool                            m_exception_caught;
    std::string                     m_test_case_name;

    static boost::scoped_ptr<unit_test_result>  m_head;
    static unit_test_result_ptr                 m_curr;
    static boost::scoped_ptr<report_formatter>  m_report_formatter;

    bool                            is_failed()   { return m_assertions_failed != m_expected_failures || m_exception_caught; }
    int                             result_code()
    { 
        return is_failed() 
                    ? ( (m_assertions_failed != 0) 
                            ? boost::exit_test_failure 
                            : boost::exit_exception_failure )
                    : boost::exit_success; 
    }
};

boost::scoped_ptr<unit_test_result> unit_test_result::Impl::m_head;
unit_test_result_ptr                unit_test_result::Impl::m_curr = unit_test_result_ptr();
boost::scoped_ptr<report_formatter> unit_test_result::Impl::m_report_formatter( new hrf_report_formatter );

//____________________________________________________________________________//

unit_test_result::unit_test_result( unit_test_result_ptr parent, std::string const& test_case_name, unit_test_counter exp_fail )
: m_pimpl( new Impl )
{
    m_pimpl->m_parent            = parent;
    m_pimpl->m_test_case_name    = test_case_name;

    m_pimpl->m_assertions_passed = 0;
    m_pimpl->m_assertions_failed = 0;
    m_pimpl->m_expected_failures = exp_fail;
    m_pimpl->m_test_cases_passed = 0;
    m_pimpl->m_test_cases_failed = 0;
    m_pimpl->m_exception_caught  = false;

}

//____________________________________________________________________________//

static void safe_delete( unit_test_result_ptr ptr ) { boost::checked_delete<unit_test_result>( ptr ); }

unit_test_result::~unit_test_result()
{
    std::list<unit_test_result_ptr>::iterator beg = m_pimpl->m_children.begin();
    std::list<unit_test_result_ptr>::iterator end = m_pimpl->m_children.end();

    std::for_each( beg, end, &safe_delete );
}

//____________________________________________________________________________//

unit_test_result&
unit_test_result::instance()
{
    assert( Impl::m_head );

    return Impl::m_curr ? *Impl::m_curr : *Impl::m_head;
}

//____________________________________________________________________________//

void
unit_test_result::test_case_start( std::string const& name, unit_test_counter expected_failures )
{
    unit_test_result_ptr new_test_case_result_inst = new unit_test_result( Impl::m_curr, name, expected_failures );

    if( Impl::m_curr )
        Impl::m_curr->m_pimpl->m_children.push_back( new_test_case_result_inst );
    else
        Impl::m_head.reset( new_test_case_result_inst );

    Impl::m_curr = new_test_case_result_inst;
}

//____________________________________________________________________________//

void
unit_test_result::test_case_end()
{
    assert( Impl::m_curr );

    Impl*                curr_impl  = Impl::m_curr->m_pimpl.get();
    unit_test_result_ptr parent     = curr_impl->m_parent;

    if( parent ) {
        // accumulate results
        parent->m_pimpl->m_assertions_passed += curr_impl->m_assertions_passed;
        parent->m_pimpl->m_assertions_failed += curr_impl->m_assertions_failed;
        parent->m_pimpl->m_test_cases_passed += curr_impl->m_test_cases_passed;
        parent->m_pimpl->m_test_cases_failed += curr_impl->m_test_cases_failed;

        // for test_cases (vs. test_suite)
        if( curr_impl->m_test_cases_passed == 0 && curr_impl->m_test_cases_failed == 0 ) {
            if( curr_impl->is_failed() )
                parent->m_pimpl->m_test_cases_failed++;
            else
                parent->m_pimpl->m_test_cases_passed++;
        }

    }

    Impl::m_curr = parent;
}

//____________________________________________________________________________//

void
unit_test_result::set_report_format( std::string const& reportformat )
{
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
    for( int i = 0; i < map_size; i++ ) {
        if( reportformat == name_value_map[i].format_name ) {
            of = name_value_map[i].format_value;
            break;
        }
    }

    if( of == HRF )
        Impl::m_report_formatter.reset( new hrf_report_formatter );
    else
        Impl::m_report_formatter.reset( new xml_report_formatter );
}

//____________________________________________________________________________//

void
unit_test_result::increase_expected_failures( unit_test_counter amount ) 
{
    m_pimpl->m_expected_failures += amount;

    if( m_pimpl->m_parent )
        m_pimpl->m_parent->increase_expected_failures( amount );
}

//____________________________________________________________________________//

void
unit_test_result::inc_failed_assertions()
{
    m_pimpl->m_assertions_failed++;
}

//____________________________________________________________________________//

void
unit_test_result::inc_passed_assertions()
{
    m_pimpl->m_assertions_passed++;
}

//____________________________________________________________________________//

void
unit_test_result::caught_exception()
{
    m_pimpl->m_exception_caught = true;
}

//____________________________________________________________________________//

std::string const&
unit_test_result::test_case_name()
{
    return m_pimpl->m_test_case_name;
}

//____________________________________________________________________________//

void
unit_test_result::reset_current_result_set()
{
    static unit_test_result_ptr backup = unit_test_result_ptr();
    static boost::scoped_ptr<unit_test_result> temporary_substitute;

    assert( Impl::m_curr );

    if( backup ) {
        Impl::m_curr = backup;
        backup = unit_test_result_ptr();
        temporary_substitute.reset();
    }
    else {
        backup = Impl::m_curr;
        Impl::m_curr = new unit_test_result( NULL, Impl::m_curr->test_case_name(), 0 );
        temporary_substitute.reset( Impl::m_curr );
    }
}

//____________________________________________________________________________//

void
unit_test_result::failures_details( unit_test_counter& num_of_failures, bool& exception_caught )
{
    num_of_failures  = m_pimpl->m_assertions_failed;
    exception_caught = m_pimpl->m_exception_caught;
}

//____________________________________________________________________________//

void
unit_test_result::report( std::string const& reportlevel, std::ostream& where_to_ )
{
    struct my_pair {
        c_string_literal    level_name;
        report_level        level_value;
    };

    static const my_pair name_value_map[] = {
        { "confirm"     , CONFIRMATION_REPORT },
        { "short"       , SHORT_REPORT },
        { "detailed"    , DETAILED_REPORT },
        { "no"          , NO_REPORT },
    };

    static int const map_size = sizeof(name_value_map)/sizeof(my_pair);

    report_level rl;
    if( reportlevel.empty() )
        rl = CONFIRMATION_REPORT;
    else {
        for( int i =0; i < map_size; i++ ) {
            if( reportlevel == name_value_map[i].level_name ) {
                rl = name_value_map[i].level_value;
                break;
            }
        }
    }

    switch( rl ) {
    case CONFIRMATION_REPORT:
        confirmation_report( where_to_ );
        break;
    case SHORT_REPORT:
    case DETAILED_REPORT:
        m_pimpl->m_report_formatter->start_result_report( where_to_ );
        report_result( where_to_, 0, rl == DETAILED_REPORT );
        m_pimpl->m_report_formatter->finish_result_report( where_to_ );
        break;
    case NO_REPORT:
        break;
    default:
        where_to_ << "*** Unrecognized report level" << std::endl;
        break;
    }
}

//____________________________________________________________________________//

void
unit_test_result::confirmation_report( std::ostream& where_to )
{
    assert( this );

    m_pimpl->m_report_formatter->start_result_report( where_to );

    m_pimpl->m_report_formatter->start_confirmation_report( where_to, 
                                                            m_pimpl->m_test_case_name, m_pimpl->m_children.empty(),
                                                            m_pimpl->m_test_cases_failed != 0 || m_pimpl->is_failed(),
                                                            m_pimpl->m_assertions_failed, m_pimpl->m_expected_failures );

    m_pimpl->m_report_formatter->finish_test_case_report( where_to, 0,
                                                          m_pimpl->m_test_case_name, m_pimpl->m_children.empty(),
                                                          m_pimpl->m_exception_caught );

    m_pimpl->m_report_formatter->finish_result_report( where_to );
}

//____________________________________________________________________________//

void
unit_test_result::report_result( std::ostream& where_to, std::size_t indent, bool detailed )
{
    assert( this );

    m_pimpl->m_report_formatter->start_test_case_report( where_to, indent, 
                                                         m_pimpl->m_test_case_name, m_pimpl->m_children.empty(),
                                                         m_pimpl->m_test_cases_failed != 0 || m_pimpl->is_failed() );

    if( m_pimpl->m_test_cases_passed + m_pimpl->m_test_cases_failed > 1 )
        m_pimpl->m_report_formatter->report_sub_test_cases_stat( where_to, indent, 
                                                                 m_pimpl->m_test_cases_passed, m_pimpl->m_test_cases_failed );

    m_pimpl->m_report_formatter->report_assertions_stat( where_to, indent, 
                                                         m_pimpl->m_assertions_passed, 
                                                         m_pimpl->m_assertions_failed, 
                                                         m_pimpl->m_expected_failures );

    if( detailed ) {
        std::list<unit_test_result_ptr>::iterator it  = m_pimpl->m_children.begin();
        std::list<unit_test_result_ptr>::iterator end = m_pimpl->m_children.end();
        
        while( it != end ) {
            unit_test_result_ptr next = *(it++);
            next->report_result( where_to, indent+2, true );
        }
    }

    m_pimpl->m_report_formatter->finish_test_case_report( where_to, indent, 
                                                          m_pimpl->m_test_case_name, m_pimpl->m_children.empty(),
                                                          m_pimpl->m_exception_caught );
}

//____________________________________________________________________________//

int
unit_test_result::result_code()
{
    return m_pimpl->result_code();
}

//____________________________________________________________________________//

} // namespace unit_test_framework

} // namespace boost

// ***************************************************************************
//  Revision History :
//  
//  $Log: unit_test_result.cpp,v $
//  Revision 1.15  2003/02/15 21:55:32  rogeeff
//  bormland warning fix
//
//  Revision 1.14  2003/02/13 08:37:59  rogeeff
//  reworked to support multiple report formats
//  C strings eliminated
//
//  Revision 1.13  2002/12/11 13:41:49  beman_dawes
//  fix missing std::
//
//  Revision 1.12  2002/12/08 18:16:57  rogeeff
//  eliminated all uses of NULL
//  switched to use c_string_literal
//
//  Revision 1.11  2002/11/02 20:04:42  rogeeff
//  release 1.29.0 merged into the main trank
//

// ***************************************************************************

// EOF
