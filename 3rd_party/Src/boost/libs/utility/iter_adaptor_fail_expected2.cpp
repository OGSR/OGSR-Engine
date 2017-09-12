//  Test boost/pending/iterator_adaptors.hpp

//  (C) Copyright Jeremy Siek 1999. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

// Revision History
// 21 Jan 01 Initial version (Jeremy Siek)

#include <boost/config.hpp>
#include <iostream>
#include <iterator>
#include <boost/pending/iterator_adaptors.hpp>

int main()
{
  typedef boost::iterator_adaptor<std::istream_iterator<int>,
    boost::default_iterator_policies,
    int,int&,int*,std::input_iterator_tag> adaptor_type;
  
  adaptor_type iter;
  --iter;
  return 0;
}
