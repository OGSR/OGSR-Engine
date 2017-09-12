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
# include <boost/preprocessor/selection.hpp>
# include <libs/preprocessor/test/test.h>

BEGIN BOOST_PP_MAX(2, 2) == 2 END
BEGIN BOOST_PP_MAX(2, 1) == 2 END
BEGIN BOOST_PP_MAX(1, 2) == 2 END

BEGIN BOOST_PP_MIN(2, 2) == 2 END
BEGIN BOOST_PP_MIN(2, 1) == 1 END
BEGIN BOOST_PP_MIN(1, 2) == 1 END
