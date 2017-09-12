/*
Copyright (C) 2002, Rene Rivera. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears in
all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.
*/

#include <cstdio>

int main(int /* argc */, char ** /* argv */)
{
    for (int i = 0; i < 16; ++i)
    {
        std::printf("%d * 2 = %d\n",i,i*2);
    }
    return 0;
}
