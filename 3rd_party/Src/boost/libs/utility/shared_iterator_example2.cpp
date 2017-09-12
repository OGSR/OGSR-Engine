// (C) Copyright Ronald Garcia 2002. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include "boost/shared_container_iterator.hpp"
#include "boost/shared_ptr.hpp"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>


template <typename Iterator>
void print_range_nl (Iterator begin, Iterator end) {
  typedef typename std::iterator_traits<Iterator>::value_type val;
  std::copy(begin,end,std::ostream_iterator<val>(std::cout,","));
  std::cout.put('\n');
}


int main() {

  typedef boost::shared_ptr< std::vector<int> > ints_t;
  {
    ints_t ints(new std::vector<int>());

    ints->push_back(0);
    ints->push_back(1);
    ints->push_back(2);
    ints->push_back(3);
    ints->push_back(4);
    ints->push_back(5);

    print_range_nl(boost::make_shared_container_iterator(ints->begin(),ints),
		   boost::make_shared_container_iterator(ints->end(),ints));
  }
  


  return 0;
}
