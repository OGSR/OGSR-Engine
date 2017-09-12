//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
//  TITLE:         template iterator-constructors
//  DESCRIPTION:   The standard library does not provide
//                 templated iterator constructors for its containers.

#include <vector>
#include <deque>
#include <list>

namespace boost_no_templated_iterator_constructors{

int test()
{
   std::vector<int> v1;
   std::deque<int>  d1;
   std::list<char>  l1;

   //
   // now try constructors:
   std::vector<long> v2(d1.begin(), d1.end());
   std::deque<long>  d2(v1.begin(), v1.end());
   std::list<long>   l2(d1.begin(), d1.end());

   return 0;
}

}



