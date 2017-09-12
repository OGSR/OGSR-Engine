// (C) Copyright Ronald Garcia 2002. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include "boost/shared_container_iterator.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/tuple/tuple.hpp" // for boost::tie
#include <algorithm>              // for std::copy
#include <iostream>              
#include <vector>


typedef boost::shared_container_iterator_generator< std::vector<int> >::type 
  function_iterator;

std::pair<function_iterator,function_iterator>
return_range() {
  boost::shared_ptr< std::vector<int> > range(new std::vector<int>());
  range->push_back(0);
  range->push_back(1);
  range->push_back(2);
  range->push_back(3);
  range->push_back(4);
  range->push_back(5);
  return boost::make_shared_container_range(range);
}


int main() {


  function_iterator i,end;
  
  boost::tie(i,end) = return_range();

  std::copy(i,end,std::ostream_iterator<int>(std::cout,","));
  std::cout.put('\n');

  return 0;
}
