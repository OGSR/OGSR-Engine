// test file for special functions.

//  (C) Copyright Hubert Holin 2003. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.


#include <iomanip>
#include <iostream>
#include <functional>


#include <boost/bind.hpp>
#include <boost/function.hpp>


#include <boost/test/unit_test_suite_ex.hpp>


#include "sinc_test.hpp"
#include "sinhc_test.hpp"
#include "atanh_test.hpp"
#include "asinh_test.hpp"
#include "acosh_test.hpp"



boost::unit_test_framework::test_suite *    init_unit_test_suite(int, char *[])
{
    //::boost::unit_test_framework::unit_test_log::instance().
    //    set_log_threshold_level_by_name("messages");
    
    boost::unit_test_framework::test_suite *    test =
        BOOST_TEST_SUITE("special_functions_test");
    
#define    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(fct,type)    \
    test->add(BOOST_TEST_CASE(::boost::bind(static_cast          \
        < void (*) (const char *) >(&fct##_test<type>), #type)));
    
    
#define    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST(type)        \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(atanh,type)    \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(asinh,type)    \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(acosh,type)    \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinc_pi,type)  \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinhc_pi,type)
    
    
#define    BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST(type)      \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinc_pi_complex,type)   \
    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR(sinhc_pi_complex,type)
    
    
#ifdef    BOOST_NO_TEMPLATE_TEMPLATES

#define    BOOST_SPECIAL_FUNCTIONS_TEST(type)                                 \
    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST(type)                                 \
    BOOST_MESSAGE("Warning: no template templates; curtailed functionality.");
    
#else    /* BOOST_NO_TEMPLATE_TEMPLATES */

#define    BOOST_SPECIAL_FUNCTIONS_TEST(type)            \
    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST(type)            \
    BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST(type)
    
#endif    /* BOOST_NO_TEMPLATE_TEMPLATES */
    
    
    BOOST_SPECIAL_FUNCTIONS_TEST(float)
    BOOST_SPECIAL_FUNCTIONS_TEST(double)
    BOOST_SPECIAL_FUNCTIONS_TEST(long double)
    
    
#undef    BOOST_SPECIAL_FUNCTIONS_TEST

#undef    BOOST_SPECIAL_FUNCTIONS_TEMPLATE_TEMPLATE_TEST
    
#undef    BOOST_SPECIAL_FUNCTIONS_COMMON_TEST

#undef    BOOST_SPECIAL_FUNCTIONS_COMMON_GENERATOR
    
    
#ifdef    BOOST_SPECIAL_FUNCTIONS_TEST_VERBOSE
        
    using    ::std::numeric_limits;
    
    BOOST_MESSAGE("epsilon");
    
    BOOST_MESSAGE( ::std::setw(15) << numeric_limits<float>::epsilon()
                << ::std::setw(15) << numeric_limits<double>::epsilon()
                << ::std::setw(15) << numeric_limits<long double>::epsilon());
    
    BOOST_MESSAGE(" ");
    
    atanh_manual_check();
    asinh_manual_check();
    acosh_manual_check();
    sinc_pi_manual_check();
    sinhc_pi_manual_check();
    
#endif    /* BOOST_SPECIAL_FUNCTIONS_TEST_VERBOSE */
    
    return(test);
}

