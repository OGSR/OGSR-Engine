//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: test_fp_comparisons.cpp,v $
//
//  Version     : $Id: test_fp_comparisons.cpp,v 1.2 2003/02/15 21:53:39 rogeeff Exp $
//
//  Description : tests floating point comparison algorithms
// ***************************************************************************

// Boost.Test
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_result.hpp>
using namespace boost::unit_test_framework;

// STL
#include <iostream>

//____________________________________________________________________________//

#define CHECK_TOOL_USAGE( tool_usage, check )               \
{                                                           \
    boost::test_toolbox::output_test_stream output;         \
                                                            \
    unit_test_log::instance().set_log_stream( output );     \
    { unit_test_result_saver saver;                         \
      tool_usage;                                           \
    }                                                       \
    unit_test_log::instance().set_log_stream( std::cout );  \
    BOOST_CHECK( check );                                   \
}

//____________________________________________________________________________//

#if !defined(__BORLANDC__)
#define CHECK_PATTERN( msg, shift ) \
    (boost::wrap_stringstream().ref() << __FILE__ << "(" << __LINE__ << "): " << msg).str()

#else

#define CHECK_PATTERN( msg, shift ) \
    (boost::wrap_stringstream().ref() << __FILE__ << "(" << (__LINE__-shift) << "): " << msg).str()

#endif
//____________________________________________________________________________//

template<typename FPT>
void
test_BOOST_CHECK_CLOSE( FPT ) {
#undef  TEST_CASE_NAME
#define TEST_CASE_NAME << '\"' << "test_BOOST_CHECK_CLOSE_all" << "\"" <<
    unit_test_log::instance().set_log_threshold_level( log_messages );

    BOOST_MESSAGE( "testing BOOST_CHECK_CLOSE for " << typeid(FPT).name() );


#define BOOST_CHECK_CLOSE_SHOULD_PASS( first, second, e )       \
    fp1     = static_cast<FPT>(first);                          \
    fp2     = static_cast<FPT>(second);                         \
    epsilon = static_cast<FPT>(e);                              \
                                                                \
    CHECK_TOOL_USAGE(                                           \
        BOOST_CHECK_CLOSE( fp1, fp2, epsilon ),                 \
        output.is_empty()                                       \
    )

#define BOOST_CHECK_CLOSE_SHOULD_PASS_N( first, second, num )   \
    fp1     = static_cast<FPT>(first);                          \
    fp2     = static_cast<FPT>(second);                         \
                                                                \
    CHECK_TOOL_USAGE(                                           \
        BOOST_CHECK_CLOSE( fp1, fp2, (num) ),                   \
        output.is_empty()                                       \
    )

#define BOOST_CHECK_CLOSE_SHOULD_FAIL( first, second, e )       \
    fp1     = static_cast<FPT>(first);                          \
    fp2     = static_cast<FPT>(second);                         \
    epsilon = static_cast<FPT>(e);                              \
                                                                \
    CHECK_TOOL_USAGE(                                           \
        BOOST_CHECK_CLOSE( fp1, fp2, epsilon ),                 \
        output.is_equal( CHECK_PATTERN( "error in " TEST_CASE_NAME ": test fp1 ~= fp2 failed [" \
                                        << fp1 << " !~= " << fp2 << " (+/-" << epsilon << ")]\n", 0 ) ) \
    )

#define BOOST_CHECK_CLOSE_SHOULD_FAIL_N( first, second, num )   \
    fp1     = static_cast<FPT>(first);                          \
    fp2     = static_cast<FPT>(second);                         \
    epsilon = num * std::numeric_limits<FPT>::epsilon()/2;      \
                                                                \
    CHECK_TOOL_USAGE(                                           \
        BOOST_CHECK_CLOSE( fp1, fp2, num ),                     \
        output.is_equal( CHECK_PATTERN( "error in " TEST_CASE_NAME ": test fp1 ~= fp2 failed [" \
        << fp1 << " !~= " << fp2 << " (+/-" << epsilon << ")]\n", 0 ) ) \
    )

    FPT fp1, fp2, epsilon, tmp;

    BOOST_CHECK_CLOSE_SHOULD_PASS( 1, 1, 0 );

    BOOST_CHECK_CLOSE_SHOULD_FAIL( 0, 1e-20, 1e-7 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 0, 1e-30, 1e-7 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 0, -1e-10, 1e-3 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 0.123456, 0.123457, 1e-6 );

    BOOST_CHECK_CLOSE_SHOULD_PASS( 0.123456, 0.123457, 1e-5 );

    BOOST_CHECK_CLOSE_SHOULD_FAIL( 0.123456, -0.123457, 1e-5 );

    BOOST_CHECK_CLOSE_SHOULD_PASS( 1.23456e28, 1.23457e28, 1e-5 );

    BOOST_CHECK_CLOSE_SHOULD_FAIL( 1.23456e-10, 1.23457e-11, 1e-5 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 1.111e-10, 1.112e-10, 0.0008999 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 1.112e-10, 1.111e-10, 0.0008999 );

    BOOST_CHECK_CLOSE_SHOULD_PASS( 1     , 1.0001, 1.1e-4 );
    BOOST_CHECK_CLOSE_SHOULD_PASS( 1.0002, 1.0001, 1.1e-4 );
    
    BOOST_CHECK_CLOSE_SHOULD_FAIL( 1     , 1.0002, 1.1e-4 );

    BOOST_CHECK_CLOSE_SHOULD_PASS_N( 1, 1+std::numeric_limits<FPT>::epsilon() / 2, 1 );
    
    tmp = static_cast<FPT>(1e-10);
    BOOST_CHECK_CLOSE_SHOULD_PASS_N( tmp+tmp, 2e-10, 1+2 );

    tmp = static_cast<FPT>(3.1);
    BOOST_CHECK_CLOSE_SHOULD_PASS_N( tmp*tmp, 9.61, 1+2 );

    tmp = 11;
    tmp /= 10;
    BOOST_CHECK_CLOSE_SHOULD_PASS_N( (tmp*tmp-tmp), 11./100, 1+3 );
    BOOST_CHECK_CLOSE_SHOULD_FAIL_N( 100*(tmp*tmp-tmp), 11, 3 );

    tmp = static_cast<FPT>(1e15+1e-10);
    BOOST_CHECK_CLOSE_SHOULD_PASS_N( tmp*tmp+tmp*tmp, 2e30+2e-20+4e5, 3+5 );

    fp1     = static_cast<FPT>(1.0001);
    fp2     = static_cast<FPT>(1001.1001);
    tmp     = static_cast<FPT>(1.0001);

    for( int i=0; i < 1000; i++ )
        fp1 = fp1 + tmp;

    CHECK_TOOL_USAGE(
        BOOST_CHECK_CLOSE( fp1, fp2, 1000 ),
        output.is_empty()
    );
}

void
test_BOOST_CHECK_CLOSE_all() {
    test_BOOST_CHECK_CLOSE<float>( (float)0 );
    test_BOOST_CHECK_CLOSE<double>( (double)0 );
    test_BOOST_CHECK_CLOSE<long double>( (long double)0 );

    double fp1     = 1.00000001;
    double fp2     = 1.00000002;
    double epsilon = 1e-8;

    CHECK_TOOL_USAGE(
        BOOST_CHECK_PREDICATE( close_at_tolerance<double>( epsilon, false ), 2, ( fp1, fp2 ) ),
        output.is_empty()
    );

    CHECK_TOOL_USAGE(
        BOOST_CHECK_CLOSE( fp1, fp2, epsilon ),
        output.is_equal( CHECK_PATTERN( "error in " TEST_CASE_NAME ": test fp1 ~= fp2 failed [" 
                                        << fp1 << " !~= " << fp2 << " (+/-" << epsilon << ")]\n", 3 ) )
    );

    fp1     = 1.23456e-10;
    fp2     = 1.23457e-10;
    epsilon = 8.1e-6;

    CHECK_TOOL_USAGE(
        BOOST_CHECK_PREDICATE( close_at_tolerance<double>( epsilon, false ), 2, ( fp1, fp2 ) ),
        output.is_empty()
    );

    CHECK_TOOL_USAGE(
        BOOST_CHECK_PREDICATE( close_at_tolerance<double>( epsilon ), 2, ( fp1, fp2 ) ),
        output.is_equal( CHECK_PATTERN( 
                    "error in " TEST_CASE_NAME ": test close_at_tolerance<double>( epsilon )(fp1, fp2) "
                    "failed for (" << fp1 << ", " << fp2 << ")\n", 4 ) )
    );
}

//____________________________________________________________________________//

test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
    test_suite* test = BOOST_TEST_SUITE("FP compare test");

    test->add( BOOST_TEST_CASE( &test_BOOST_CHECK_CLOSE_all ) );

    return test;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: test_fp_comparisons.cpp,v $
//  Revision 1.2  2003/02/15 21:53:39  rogeeff
//  cwpro8 fix
//
//  Revision 1.1  2003/02/13 08:47:11  rogeeff
//  *** empty log message ***
//

// ***************************************************************************

// EOF
