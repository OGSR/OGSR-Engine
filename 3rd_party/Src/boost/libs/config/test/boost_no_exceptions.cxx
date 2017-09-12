//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_EXCEPTIONS
//  TITLE:         exception handling support
//  DESCRIPTION:   The compiler in its current translation mode supports
//                 exception handling.


namespace boost_no_exceptions{

void throw_it(int i)
{
   throw i;
}

int test()
{
   try
   {
      throw_it(2);
   }
   catch(int i)
   {
      return (i == 2) ? 0 : -1;
   }
   catch(...)
   {
      return -1;
   }
   return -1;
}

}




