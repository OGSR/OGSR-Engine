//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_MSVC_STD_ITERATOR
//  TITLE:         microsoft's version of std::iterator
//  DESCRIPTION:   Microsoft's broken version of std::iterator
//                 is being used.

#include <iterator>

namespace boost_msvc_std_iterator{

int test()
{
   typedef std::iterator<
      std::random_access_iterator_tag,
      int
      > iterator_type_2;
   typedef std::reverse_iterator<const char*, const char> r_it;

   iterator_type_2::value_type v2 = 0;
   iterator_type_2::iterator_category cat2;
   //
   // suppress some warnings:
   //
   (void)v2;
   (void)cat2;

   return 0;
}

}



