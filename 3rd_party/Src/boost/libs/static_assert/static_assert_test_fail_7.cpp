//  (C) Copyright John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <climits>
#include <boost/limits.hpp>
#include <boost/static_assert.hpp>

template <class UnsignedInt>
class myclass
{
private:
   BOOST_STATIC_ASSERT(sizeof(UnsignedInt) * CHAR_BIT >= 16);
   BOOST_STATIC_ASSERT(std::numeric_limits<UnsignedInt>::is_specialized
                        && std::numeric_limits<UnsignedInt>::is_integer
                        && !std::numeric_limits<UnsignedInt>::is_signed);
public:
   /* details here */
};

myclass<int>           m2; // this should fail
myclass<unsigned char> m3; // and so should this

int main()
{
   return 0;
}
