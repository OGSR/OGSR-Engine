/*
Copyright (C) 2002, Rene Rivera. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears in
all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.
*/

#include <cstdio>

#include "simple.h"

int main(int /* argc */, char ** /* argv */)
{
    for (int i = 0; i < 16; ++i)
    {
        std::printf("%d * 2 = %d\n",i,lib_call(i));
    }
    return 0;
}
