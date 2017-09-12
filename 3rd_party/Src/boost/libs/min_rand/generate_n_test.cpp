//  generate_n test of min_rand  ---------------------------------------------//

//  (C) Copyright Beman Dawes 1999. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  Revision History
//   9 Jan 00 Correct header name (Andreas Scherer) 
//  27 May 99 Initial version

#include <boost/min_rand.hpp>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
using boost::min_rand;
using namespace std;

int main() {
  vector<long> v;
  generate_n( inserter(v, v.begin()), 10, min_rand() );
  assert( v.size() == 10 );
  copy( v.begin(), v.end(), ostream_iterator<long>(cout, "\n") );
  return 0;
  } // main
