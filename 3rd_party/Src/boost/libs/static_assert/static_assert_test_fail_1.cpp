//  (C) Copyright Steve Cleary & John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/static_assert.hpp>

//
// all these tests should fail:
//

// Namespace scope
BOOST_STATIC_ASSERT(sizeof(int) == sizeof(char)); // will not compile





