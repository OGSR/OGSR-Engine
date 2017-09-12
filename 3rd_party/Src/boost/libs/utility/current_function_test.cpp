#if defined(_MSC_VER) && !defined(__ICL)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  current_function_test.cpp - a test for boost/current_function.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#include <boost/current_function.hpp>
#include <cstdio>

void message(char const * file, long line, char const * func, char const * msg)
{
    std::printf("%s(%ld): %s in function '%s'\n", file, line, msg, func);
}

#define MESSAGE(msg) message(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, msg)

int main()
{
    MESSAGE("assertion failed");
}
