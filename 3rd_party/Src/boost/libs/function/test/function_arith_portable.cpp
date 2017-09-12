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

float mul_ints(int x, int y) { return ((float)x) * y; }
struct int_div { 
  float operator()(int x, int y) const { return ((float)x)/y; }; 
};
int main()
{
    boost::function2<float, int, int> f;
    f = int_div();
    std::cout << f(5, 3) << std::endl;
    if (f)
  std::cout << f(5, 3) << std::endl;
else
  std::cout << "f has no target, so it is unsafe to call" << std::endl;
    f = 0;
    f = &mul_ints;

    return 0;
}
