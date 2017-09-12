
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_CHECK_TYPE_HPP
#define BOOST_CHECK_TYPE_HPP

#include <boost/test/test_tools.hpp>
#include <boost/type_traits/is_same.hpp>

/*
macro:
BOOST_CHECK_TYPE(type_expression, expected_type)

type_expression:  an expression that evaluates to a typename.
expected_value:   the type we expect to find.
*/

#ifdef __BORLANDC__
#pragma option -w-8008 -w-8066 -w-8019
#endif


#define BOOST_CHECK_TYPE(type_expression, expected_type)\
do{\
   if(!::boost::is_same< type_expression, expected_type >::value){\
   BOOST_CHECK_MESSAGE(false, "The expression: \"" << BOOST_STRINGIZE(expression)\
      << "\" did not have the expected type:\n\tevaluating:   boost::is_same<"\
      << BOOST_STRINGIZE(type_expression) << ", " << BOOST_STRINGIZE(expected_type)\
      << ">" << "\n\tfound:        "\
      << typeid(::boost::is_same< type_expression, expected_type >).name());\
   }else\
      BOOST_CHECK_MESSAGE(true, "Validating Type Expression: \""\
         << BOOST_STRINGIZE(expression) << "\"");\
}while(0)

#endif

