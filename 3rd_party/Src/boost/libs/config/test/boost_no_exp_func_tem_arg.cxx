//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
//  TITLE:         non-deduced function template parameters
//  DESCRIPTION:   Can only use deduced template arguments when 
//                 calling function template instantiations.

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1200)
#error "This is known to be buggy under VC6"
#endif


namespace boost_no_explicit_function_template_arguments{

template <int i>
bool foo_17041(int j)
{
   return (i == j);
}

int test()
{
   if(0 == foo_17041<8>(8)) return -1;
   if(0 == foo_17041<4>(4)) return -1;
   if(0 == foo_17041<5>(5)) return -1;
   return 0;
}

}



