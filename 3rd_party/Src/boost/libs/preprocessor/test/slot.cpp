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
# include <boost/preprocessor/slot.hpp>
# include <libs/preprocessor/test/test.h>

# define X() 4

# define BOOST_PP_VALUE 1 + 2 + 3 + X()
# include BOOST_PP_ASSIGN_SLOT(1)

# undef X

BEGIN BOOST_PP_SLOT(1) == 10 END

# define BOOST_PP_VALUE BOOST_PP_SLOT(1) * BOOST_PP_SLOT(1)
# include BOOST_PP_ASSIGN_SLOT(1)

BEGIN BOOST_PP_SLOT(1) == 100 END
