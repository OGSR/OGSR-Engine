//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_FUNCTION_TEMPLATE_ORDERING
//  TITLE:         no function template ordering
//  DESCRIPTION:   The compiler does not perform 
//                 function template ordering or its function
//                 template ordering is incorrect.
//  
//                 template<typename T> void f(T); // #1
//                 template<typename T, typename U> void f(T (*)(U)); // #2
//                 void bar(int);
//                 f(&bar); // should choose #2.


namespace boost_no_function_template_ordering{

template<typename T>
bool f(T)
{
   return false;
}

template<typename T, typename U>
bool f(T (*)(U))
{
   return true;
}

void bar(int)
{
}

int test()
{
   int i = 0;
   return f(i) || !f(&bar);
}

}




