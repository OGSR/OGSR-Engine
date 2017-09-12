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
# include <boost/preprocessor/debug.hpp>
# include <libs/preprocessor/test/test.h>

BEGIN sizeof(BOOST_PP_ASSERT_MSG(0, "text") "") / sizeof(char) != 1 END
BEGIN sizeof(BOOST_PP_ASSERT_MSG(1, "text") "") / sizeof(char) == 1 END

BOOST_PP_ASSERT(10)

# line BOOST_PP_LINE(100, __FILE__)
BEGIN __LINE__ == 100 END
