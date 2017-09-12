// (C) Copyright Ronald Garcia 2002. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include "boost/shared_container_iterator.hpp"
#include "boost/shared_ptr.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

typedef boost::shared_container_iterator_generator< std::vector<int> >::type
  iterator;


void set_range(iterator& i, iterator& end)  {

  boost::shared_ptr< std::vector<int> > ints(new std::vector<int>());
  
  ints->push_back(0);
  ints->push_back(1);
  ints->push_back(2);
  ints->push_back(3);
  ints->push_back(4);
  ints->push_back(5);
  
  i = iterator(ints->begin(),ints);
  end = iterator(ints->end(),ints);
}


int main() {

  iterator i,end;

  set_range(i,end);

  std::copy(i,end,std::ostream_iterator<int>(std::cout,","));
  std::cout.put('\n');

  return 0;
}
