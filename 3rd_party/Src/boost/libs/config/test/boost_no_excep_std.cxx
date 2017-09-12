//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_EXCEPTION_STD_NAMESPACE
//  TITLE:         <exception> is in namespace std
//  DESCRIPTION:   Some std libs don't put the contents of
//                 <exception> in namespace std.

#include <exception>

namespace boost_no_exception_std_namespace{

int t(int i)
{
   if(i)
      std::terminate();
   return 0;
}

int test()
{
   return t(0);
}

}





