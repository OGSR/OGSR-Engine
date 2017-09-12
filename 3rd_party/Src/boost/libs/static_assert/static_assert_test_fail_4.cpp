//  (C) Copyright Steve Cleary & John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/config.hpp>
#include <boost/static_assert.hpp>

//
// all these tests should fail:
//


struct Bob
{
  public:

  // Member function scope: provides access to member variables
  int x;
  char c;
  int f()
  {
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1200 // broken sizeof in VC6
    BOOST_STATIC_ASSERT(sizeof(x) == 4);
    BOOST_STATIC_ASSERT(sizeof(c) == 1);
    BOOST_STATIC_ASSERT((sizeof(x) == sizeof(c))); // should not compile
#endif
    return x;
  }
};





