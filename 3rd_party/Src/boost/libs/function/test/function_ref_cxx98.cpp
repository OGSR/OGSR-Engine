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


struct stateful_type { int operator()(int) const { return 0; } };

int main()
{
    stateful_type a_function_object;
  boost::function<int (int)> f;
  f = boost::ref(a_function_object);

  boost::function<int (int)> f2(f);

    return 0;
}
