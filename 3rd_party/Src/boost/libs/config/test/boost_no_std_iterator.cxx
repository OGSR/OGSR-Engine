//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_ITERATOR
//  TITLE:         std::iterator
//  DESCRIPTION:   The C++ implementation fails to provide the
//                 std::iterator class.

#include <iterator>
#include <stddef.h>

namespace boost_no_std_iterator{


int test()
{
   typedef std::iterator<
      std::random_access_iterator_tag,
      int,
      ptrdiff_t,
      int*,
      int&
      > iterator_type;

   iterator_type::value_type v = 0;
   iterator_type::difference_type d = 0;
   iterator_type::pointer p = &v;
   iterator_type::reference r = v;
   iterator_type::iterator_category cat;

   typedef std::iterator<
      std::random_access_iterator_tag,
      int
      > iterator_type_2;

   iterator_type_2::value_type v2 = 0;
   iterator_type_2::difference_type d2 = 0;
   iterator_type_2::pointer p2 = &v2;
   iterator_type_2::reference r2 = v2;
   iterator_type_2::iterator_category cat2;
   //
   // suppress some warnings:
   //
   (void) &v;
   (void) &d;
   (void) &p;
   (void) &r;
   (void) &cat;

   (void) &v2;
   (void) &d2;
   (void) &p2;
   (void) &r2;
   (void) &cat2;

   return 0;
}

}



