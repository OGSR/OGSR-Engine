//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_USING_TEMPLATE
//  TITLE:         using template declarations
//  DESCRIPTION:   The compiler will not accept a using declaration
//                 that imports a class or function template
//                 into a named namespace.  Probably Borland/MSVC6 specific.

template <class T>
int global_foo(T)
{
   return 0;
}

template <class T, class U = void>
struct op
{
   friend op<T,U> operator +(const op&, const op&)
   {
      return op();
   };
};

namespace boost_no_using_template{

using ::global_foo;
using ::op;

int test()
{
   boost_no_using_template::op<int, int> a;
   boost_no_using_template::op<int, int> b;
   a+b;
   return boost_no_using_template::global_foo(0);
}

}




