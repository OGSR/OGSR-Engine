//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_UNREACHABLE_RETURN_DETECTION
//  TITLE:         detection of unreachable returns
//  DESCRIPTION:   If a return is unreachable, then no return
//                 statement should be required, however some
//                 compilers insist on it, while other issue a
//                 bunch of warnings if it is in fact present.


namespace boost_no_unreachable_return_detection{

int checker()
{
   throw 0;
   // no return statement: we don't ever get here...
}

int check = 0;

int test()
{
   if(check)
      return checker();
   return 0;
}

}


