//  Copyright (c) 2003 boost.org
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  http://www.boost.org
// 

#if SELECT == 1
void z1() {}
#elif SELECT == 2
void z2() {}
#else
#error Invlid value of SELECT
#endif
