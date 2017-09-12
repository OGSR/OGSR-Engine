//  (C) Copyright Steve Cleary & John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <climits>
#include <cwchar>
#include <boost/static_assert.hpp>

#if !defined(WCHAR_MIN)
#define WCHAR_MIN 0
#endif

namespace boost{

namespace my_conditions {
BOOST_STATIC_ASSERT(sizeof(int) * CHAR_BIT >= 32);
BOOST_STATIC_ASSERT(WCHAR_MIN >= 0);

} // namespace my_conditions

} // namespace boost

int main()
{
   return 0;
}


