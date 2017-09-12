//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_SGI_TYPE_TRAITS
//  TITLE:         SGI style <type_traits.h>
//  DESCRIPTION:   The standard library has it's own type_traits implementation.

#include <type_traits.h>


namespace boost_has_sgi_type_traits{

struct foo_type{};

int test()
{
   ::__true_type t;
   ::__false_type f;
   typedef ::__type_traits<int>::has_trivial_destructor td;
   typedef ::__type_traits<double>::has_trivial_assignment_operator ta;
   typedef ::__type_traits<float>::has_trivial_copy_constructor tc;
   typedef ::__type_traits<char>::has_trivial_default_constructor tdc;
   typedef ::__type_traits<foo_type>::is_POD_type isp;

   (void) &t;   // avoid "unused variable" warnings
   (void) &f;

   return 0;
}

}




