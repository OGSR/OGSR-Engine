//  boost class noncopyable test program  ------------------------------------//

//  (C) Copyright boost.org 1999. Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies. This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

//  See http://www.boost.org for most recent version including documentation.

//  Revision History
//   9 Jun 99  Add unnamed namespace
//   2 Jun 99  Initial Version

#include <boost/noncopyable.hpp>
#include <iostream>

//  This program demonstrates compiler errors resulting from trying to copy
//  construct or copy assign a class object derived from class noncopyable.

namespace
{
    class DontTreadOnMe : private boost::noncopyable
    {
    public:
         DontTreadOnMe() { std::cout << "defanged!" << std::endl; }
    };   // DontTreadOnMe

}   // unnamed namespace

int main()
{
    DontTreadOnMe object1;
    DontTreadOnMe object2(object1);
    object1 = object2;
    return 0;
}   // main
  
