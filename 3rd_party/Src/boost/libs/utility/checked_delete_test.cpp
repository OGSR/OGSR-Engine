//  Boost checked_delete test program  ---------------------------------------//

//  (C) Copyright Beman Dawes 2001. Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies. This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

//  See http://www.boost.org for most recent version including documentation.

//  Revision History
//  21 May 01  Initial version (Beman Dawes)

#include <boost/checked_delete.hpp>  // for checked_delete

//  This program demonstrates compiler errors when trying to delete an
//  incomplete type.

namespace
{
    class Incomplete;
}

int main()
{
    Incomplete * p;
    boost::checked_delete(p);          // should cause compile time error
    Incomplete ** pa;
    boost::checked_array_delete(pa);   // should cause compile time error
    return 0;
}   // main
