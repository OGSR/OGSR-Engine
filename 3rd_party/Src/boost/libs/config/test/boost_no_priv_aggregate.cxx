//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_PRIVATE_IN_AGGREGATE
//  TITLE:         private in aggregate types
//  DESCRIPTION:   The compiler misreads 8.5.1, treating classes
//                 as non-aggregate if they contain private or
//                 protected member functions.


namespace boost_no_private_in_aggregate{

struct t
{
private:
   void foo(){ i = j; }
public:
   void uncallable(); // silences warning from GCC
   int i;
   int j;
};


int test()
{
   t inst = { 0, 0, };
   (void) &inst;      // avoid "unused variable" warning
   return 0;
}

}



