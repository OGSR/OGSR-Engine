// Function library

// Copyright (C) 2001-2003 Douglas Gregor

// Permission to copy, use, sell and distribute this software is granted 
// provided this copyright notice appears in all copies. Permission to modify 
// the code and to distribute modified code is granted provided this copyright 
// notice appears in all copies, and a notice that the code was modified is 
// included with the copyright notice. This software is provided "as is" 
// without express or implied warranty, and with no claim as to its 
// suitability for any purpose. 

// For more information, see http://www.boost.org/

    
#include <boost/function.hpp>
#include <iostream>
#include <functional>

struct X {
  int foo(int);
};
int X::foo(int x) { return -x; }

int main()
{
    boost::function2<int, X*, int> f;

f = &X::foo;
  
X x;
f(&x, 5);

    return 0;
}
