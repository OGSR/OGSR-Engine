//  min_rand_test program  ---------------------------------------------------//

//  (C) Copyright Beman Dawes 1998. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  Revision History
//   7 Feb 00 Remove VC++ 5.0 <iostream> problem workaround (Paul Bristow)
//   9 Jan 00 Correct header name (Andreas Scherer) 
//   9 Nov 98 Initial version

#include <boost/min_rand.hpp>
using boost::min_rand;

#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

//  template class used to verify RandomNumberGenerator portion of interface  //

template<class RandomNumberGenerator>
void try_rng( RandomNumberGenerator& rng ) {
  typename RandomNumberGenerator::argument_type mod = 1000;
  typename RandomNumberGenerator::result_type result;
  for ( int i = 1; i <= 10000; ++i ) {
    result = rng(mod);
    assert( result >= 0 && result < mod );
    } // for
  } // try_rng

//  test program  ------------------------------------------------------------//

int main() {

  min_rand rng;
  assert( rng == 1 );

  for ( int i = 1; i <= 10000; ++i )
   { ++rng; }

  cout << "actual: " << rng << " correct: " << rng.ten_thousandth() << endl;
  assert( rng == rng.ten_thousandth() );

  long tl = rng;
  assert( tl == rng++ );
  assert( tl != rng );

  rng = 1234567890;
  assert( rng == 1234567890 );

  tl = rng;
  assert( tl == 1234567890 );

  double td = rng.fvalue();
  assert( td == double(rng) / 2147483647L );

  min_rand copied( rng );
  assert( copied == 1234567890 );
  rng = 987654321;
  copied = rng;
  assert( copied == 987654321 );

  rng = 1;
  try_rng( rng );
  cout << "actual: " << rng << " correct: " << rng.ten_thousandth() << endl;
  assert( rng == rng.ten_thousandth() );

  return 0;
  } // main
