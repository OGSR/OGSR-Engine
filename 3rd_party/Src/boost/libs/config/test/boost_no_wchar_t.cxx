//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_INTRINSIC_WCHAR_T
//  TITLE:         intrinsic wchar_t
//  DESCRIPTION:   The C++ implementation does not provide wchar_t,
//                 or it is really a synonym for another integral type.
//                 Use this symbol to decide whether it is appropriate
//                 to explicitly specialize a template on wchar_t if there
//                 is already a specialization for other integer types.

#ifndef BOOST_NO_CWCHAR
#include <wchar.h>
#endif

namespace boost_no_intrinsic_wchar_t{

template <class T>
struct is_int{ int i; };

template <> struct is_int<unsigned char>{ int i; };
template <> struct is_int<signed char>{ int i; };
template <> struct is_int<char>{ int i; };
template <> struct is_int<unsigned short>{ int i; };
template <> struct is_int<short>{ int i; };
template <> struct is_int<unsigned int>{ int i; };
template <> struct is_int<int>{ int i; };
template <> struct is_int<unsigned long>{ int i; };
template <> struct is_int<long>{ int i; };
template <> struct is_int<wchar_t>{ int i; };

int test()
{
   return 0;
}

}



