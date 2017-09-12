// test file for quaternion.hpp

//  (C) Copyright Hubert Holin 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.


#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite_ex.hpp>

#include "quaternion_mi1.h"
#include "quaternion_mi2.h"


boost::unit_test_framework::test_suite *    init_unit_test_suite(int, char *[])
{
    ::boost::unit_test_framework::unit_test_log::instance().
            set_log_threshold_level_by_name("messages");
    
    boost::unit_test_framework::test_suite *    test =
        BOOST_TEST_SUITE("quaternion_multiple_inclusion_test");
    
    test->add(BOOST_TEST_CASE(&quaternion_mi1));
    test->add(BOOST_TEST_CASE(&quaternion_mi2));
    
    return(test);
}

