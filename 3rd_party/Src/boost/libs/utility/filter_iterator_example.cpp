// Example of using the filter iterator adaptor from
// boost/iterator_adaptors.hpp.

//  (C) Copyright Jeremy Siek 1999. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <boost/config.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <boost/iterator_adaptors.hpp>

struct is_positive_number {
  bool operator()(int x) { return 0 < x; }
};

int main()
{
  int numbers_[] = { 0, -1, 4, -3, 5, 8, -2 };
  const int N = sizeof(numbers_)/sizeof(int);
  
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
  // Assume there won't be proper iterator traits for pointers. This
  // is just a wrapper for int* which has the right traits.
  typedef boost::iterator_adaptor<int*, boost::default_iterator_policies, int> base_iterator;
#else
  typedef int* base_iterator;
#endif
  base_iterator numbers(numbers_);
  
  // Example using make_filter_iterator()
  std::copy(boost::make_filter_iterator<is_positive_number>(numbers, numbers + N),
            boost::make_filter_iterator<is_positive_number>(numbers + N, numbers + N),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Example using filter_iterator_generator
  typedef boost::filter_iterator_generator<is_positive_number, base_iterator, int>::type
    FilterIter;
  is_positive_number predicate;
  FilterIter::policies_type policies(predicate, numbers + N);
  FilterIter filter_iter_first(numbers, policies);
  FilterIter filter_iter_last(numbers + N, policies);

  std::copy(filter_iter_first, filter_iter_last, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Another example using make_filter_iterator()
  std::copy(boost::make_filter_iterator(numbers, numbers + N, 
                                        std::bind2nd(std::greater<int>(), -2)),
            boost::make_filter_iterator(numbers + N, numbers + N, 
                                        std::bind2nd(std::greater<int>(), -2)),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  
  
  return 0;
}
