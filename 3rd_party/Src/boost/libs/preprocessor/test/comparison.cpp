# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.  Permission to copy, use,        *
#  *     modify, sell, and distribute this software is granted provided       *
#  *     this copyright notice appears in all copies.  This software is       *
#  *     provided "as is" without express or implied warranty, and with       *
#  *     no claim at to its suitability for any purpose.                      *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# include <boost/preprocessor/comparison.hpp>
# include <libs/preprocessor/test/test.h>

// equality

BEGIN BOOST_PP_EQUAL(2, 0) == 0 END
BEGIN BOOST_PP_EQUAL(2, 2) == 1 END

// inequality

BEGIN BOOST_PP_NOT_EQUAL(2, 0) == 1 END
BEGIN BOOST_PP_NOT_EQUAL(2, 2) == 0 END

// less

BEGIN BOOST_PP_LESS(2, 1) == 0 END
BEGIN BOOST_PP_LESS(1, 2) == 1 END

// less_equal

BEGIN BOOST_PP_LESS_EQUAL(2, 1) == 0 END
BEGIN BOOST_PP_LESS_EQUAL(1, 2) == 1 END
BEGIN BOOST_PP_LESS_EQUAL(2, 2) == 1 END

// greater

BEGIN BOOST_PP_GREATER(2, 1) == 1 END
BEGIN BOOST_PP_GREATER(1, 2) == 0 END

// greater_equal

BEGIN BOOST_PP_GREATER_EQUAL(2, 1) == 1 END
BEGIN BOOST_PP_GREATER_EQUAL(1, 2) == 0 END
BEGIN BOOST_PP_GREATER_EQUAL(2, 2) == 1 END
