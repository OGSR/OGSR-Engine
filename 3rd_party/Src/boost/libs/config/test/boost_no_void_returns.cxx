//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_VOID_RETURNS
//  TITLE:         no void returns
//  DESCRIPTION:   The compiler does not allow a void function 
//                 to return the result of calling another void
//                 function.
//  
//                 void f() {}
//                 void g() { return f(); }


namespace boost_no_void_returns{

void f() {}

void g() { return f(); }

int test()
{
    return 0;
}

}




