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
# include <boost/preprocessor/cat.hpp>
# include <boost/preprocessor/facilities.hpp>
# include <libs/preprocessor/test/test.h>

BEGIN BOOST_PP_APPLY(BOOST_PP_NIL) 0 == 0 END
BEGIN BOOST_PP_APPLY((0)) == 0 END

BEGIN BOOST_PP_APPLY((BOOST_PP_EMPTY))() 0 == 0 END

# define MACRO(x, y, z) 1
# define ARGS (1, 2, 3)

BEGIN BOOST_PP_EXPAND(MACRO ARGS) == 1 END

BEGIN BOOST_PP_IDENTITY(1)() == 1 END

BEGIN BOOST_PP_CAT(BOOST_PP_INTERCEPT, 2) 1 == 1 END
