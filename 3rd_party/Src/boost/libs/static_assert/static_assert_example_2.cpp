//  (C) Copyright John Maddock 2000.
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <iterator>
#include <list>
#include <deque>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

template <class RandomAccessIterator >
RandomAccessIterator foo(RandomAccessIterator from, RandomAccessIterator to)
{
   // this template can only be used with
   // random access iterators...
   typedef typename std::iterator_traits< RandomAccessIterator >::iterator_category cat;
   BOOST_STATIC_ASSERT((boost::is_convertible<cat, const std::random_access_iterator_tag&>::value));
   //
   // detail goes here...
   return from;
}

int main()
{
   std::deque<int> d;
   std::list<int> l;
   foo(d.begin(), d.end()); // OK
   //foo(l.begin(), l.end()); // error
   return 0;
}






