//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
//  TITLE:         Koenig lookup
//  DESCRIPTION:   Compiler does not implement
//                 argument-dependent lookup (also named Koenig lookup); 
//                 see std::3.4.2 [basic.koenig.lookup]


namespace boost_no_argument_dependent_lookup{

namespace foobar{

struct test{};

void call_test(const test&)
{}

}

int test()
{
   foobar::test t;
   call_test(t);
   return 0;
}


}


