//  (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.
//
// This is an example demonstrating how to use the tie() function.
// The purpose of tie() is to make it easiery to deal with std::pair
// return values.
//
// Contributed by Jeremy Siek
//
// Sample output
//
// 3 successfully inserted.
// 9 was already in the set.
// There were 2 occurrences of 4.

#include <set>
#include <algorithm>
#include <iostream>
#include <iterator>   // std::distance
// Note: tie() use to live in boost/utility.hpp, but
// not it is part of the more general Boost Tuple Library.
#include <boost/tuple/tuple.hpp>

int
main(int, char*[])
{
  {
    typedef std::set<int> SetT;
    SetT::iterator i;
    bool inserted;
    
    int vals[5] = { 5, 2, 4, 9, 1 };
    SetT s(vals, vals + 5);
    
    // Using tie() with a return value of pair<iterator,bool>

    int new_vals[2] = { 3, 9 };

    for (int k = 0; k < 2; ++k) {
      boost::tie(i,inserted) = s.insert(new_vals[k]);
      if (!inserted)
        std::cout << *i << " was already in the set." << std::endl;
      else
        std::cout << *i << " successfully inserted." << std::endl;    
    }
  }    
  {
    int* i, *end;
    int vals[6] = { 5, 2, 4, 4, 9, 1 };
    std::sort(vals, vals + 6);

    // Using tie() with a return value of pair<iterator,iterator>

    boost::tie(i,end) = std::equal_range(vals, vals + 6, 4);
    std::cout << "There were " << std::distance(i,end)
              << " occurrences of " << *i << "." << std::endl;
    // Footnote: of course one would normally just use std::count()
    // to get this information, but that would spoil the example :)
  }
  return 0;
}
