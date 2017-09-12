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
# include <boost/preprocessor/array.hpp>
# include <libs/preprocessor/test/test.h>

# define ARRAY (3, (0, 1, 2))

// element access

BEGIN BOOST_PP_ARRAY_ELEM(1, ARRAY) == 1 END
BEGIN BOOST_PP_ARRAY_ELEM(2, (5, (0, 1, 2, 3, 4))) == 2 END

// size

BEGIN BOOST_PP_ARRAY_SIZE(ARRAY) == 3 END
BEGIN BOOST_PP_ARRAY_SIZE((5, (0, 1, 2, 3, 4))) == 5 END
