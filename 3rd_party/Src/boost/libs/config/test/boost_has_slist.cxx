//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_SLIST
//  TITLE:         <slist>
//  DESCRIPTION:   The C++ implementation provides the (SGI) slist class.

#include <slist>

#ifndef BOOST_STD_EXTENSION_NAMESPACE
#define BOOST_STD_EXTENSION_NAMESPACE std
#endif

namespace boost_has_slist{

template <class T, class Alloc>
void foo(const BOOST_STD_EXTENSION_NAMESPACE::slist<T,Alloc>& )
{
}


int test()
{
   BOOST_STD_EXTENSION_NAMESPACE::slist<int> l;
   foo(l);
   return 0;
}

}





