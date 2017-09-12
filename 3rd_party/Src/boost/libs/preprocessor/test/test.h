# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Permission to copy, use, modify, sell and distribute this software is
#  * granted provided this copyright notice appears in all copies. This
#  * software is provided "as is" without express or implied warranty, and
#  * with no claim as to its suitability for any purpose.
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_LIBS_PREPROCESSOR_REGRESSION_TEST_H
# define BOOST_LIBS_PREPROCESSOR_REGRESSION_TEST_H
#
# include <boost/preprocessor/cat.hpp>
#
# define BEGIN typedef int BOOST_PP_CAT(test_, __LINE__)[((
# define END )==1) ? 1 : -1];

#include <cstdio>

namespace std { }
using namespace std;

int main(void) {
	printf("pass " __TIME__);
	return 0;
}

# endif
