//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_DEPENDENT_NESTED_DERIVATIONS
//  TITLE:         dependent nested template classes
//  DESCRIPTION:   The compiler fails to compile
//                 a nested class that has a dependent base class:
//                 template<typename T>
//                 struct foo : {
//                    template<typename U>
//                    struct bar : public U {};
//                 };
#ifndef BOOST_NESTED_TEMPLATE
#define BOOST_NESTED_TEMPLATE template
#endif


namespace boost_no_dependent_nested_derivations{

struct UDT1{};
struct UDT2{};

template<typename T> 
struct foo 
{
  template<typename U> 
  struct bar : public foo<U> 
  {};
};

template <class T>
void foo_test(T)
{
   typedef foo<T> foo_type;
   typedef typename foo_type::BOOST_NESTED_TEMPLATE bar<UDT2> bar_type;
   foo<T> ft;
   bar_type bt;
   (void) &bt;
   (void) &ft;
}

int test()
{
   foo_test(UDT1());
   return 0;
}

}





