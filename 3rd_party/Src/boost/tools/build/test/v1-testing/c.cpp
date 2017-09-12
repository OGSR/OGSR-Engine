// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <iostream>

#ifndef RESULTCODE
# define RESULTCODE 0
#endif

int main()
{
    std::cout << "returning result: " << RESULTCODE << std::endl;
    return RESULTCODE;
}

