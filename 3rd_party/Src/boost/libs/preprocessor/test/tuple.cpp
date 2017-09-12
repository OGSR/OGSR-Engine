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
# include <boost/preprocessor/tuple.hpp>
# include <libs/preprocessor/test/test.h>

# define TUPLE (0, 1, 2, 3, 4, 5)

BEGIN BOOST_PP_TUPLE_ELEM(6, 3, TUPLE) == 3 END
BEGIN BOOST_PP_TUPLE_ELEM(6, 5, TUPLE) == 5 END

# define CALC(x) BOOST_PP_TUPLE_ELEM(3, 0, x) BOOST_PP_TUPLE_ELEM(3, 1, x) BOOST_PP_TUPLE_ELEM(3, 2, x)
# define T2 (+3, /2, +6)

BEGIN CALC(T2) == 7 END
BEGIN CALC(BOOST_PP_TUPLE_REVERSE(3, T2)) == 6 END
