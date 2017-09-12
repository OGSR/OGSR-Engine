//  (C) Copyright Steve Cleary & John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/static_assert.hpp>

//
// this tests should fail:
//

struct Bob
{
  private:  // can be in private, to avoid namespace pollution
    BOOST_STATIC_ASSERT(sizeof(int) == sizeof(char)); // will not compile
  public:

  // Member function scope: provides access to member variables
  int x;
  char c;
  int f()
  {
#ifndef _MSC_VER // broken sizeof in VC6
    BOOST_STATIC_ASSERT(sizeof(x) == 4);
    BOOST_STATIC_ASSERT(sizeof(c) == 1);
#endif
    //BOOST_STATIC_ASSERT((sizeof(x) == sizeof(c))); // should not compile
    return x;
  }
};





