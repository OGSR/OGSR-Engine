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
#include <list>
#include <boost/pending/iterator_adaptors.hpp>

int main()
{
  typedef boost::iterator_adaptor<std::list<int>::iterator,
    boost::default_iterator_policies,
    int,int&,int*,std::bidirectional_iterator_tag> adaptor_type;
  
  adaptor_type i;
  i += 4;
  return 0;
}
