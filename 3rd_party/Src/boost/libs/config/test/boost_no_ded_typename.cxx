//  (C) Copyright John Maddock and David Abrahams 2002. Permission to
//  copy, use, modify, sell and distribute this software is granted
//  provided this copyright notice appears in all copies. This
//  software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_DEDUCED_TYPENAME
//  TITLE:         deduced typenames
//  DESCRIPTION:   Some compilers can't use the typename keyword in deduced contexts.

#ifndef BOOST_DEDUCED_TYPENAME
#define BOOST_DEDUCED_TYPENAME typename
#endif


namespace boost_deduced_typename{

template <class T>
int f(T const volatile*, BOOST_DEDUCED_TYPENAME T::type* = 0)
{
        return 0;
}

struct X { typedef int type; };

int test()
{
   return f((X*)0);
}

}







