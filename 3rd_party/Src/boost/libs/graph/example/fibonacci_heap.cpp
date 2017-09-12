//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#ifdef __GNUC__
#define __STL_NO_DRAND48
#endif
#include <algorithm>
#include <boost/pending/fibonacci_heap.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/pending/indirect_cmp.hpp>

using namespace boost;

int
main()
{
  typedef std::vector<int> ValArray;
  typedef indirect_cmp<ValArray::const_iterator,std::less<int> > Cmp;
  int i;
  for (int N = 2; N < 200; ++N) {
    for (int t = 0; t < 10; ++t) {
      fibonacci_heap<int, std::less<int> > Q(N, std::less<int>());
      std::vector<int> v, w(N);

      for (int c = 0; c < w.size(); ++c)
        w[c] = c;
      std::random_shuffle(w.begin(), w.end());
      
      for (i = 0; i < N; ++i)
        Q.push(w[i]);

      for (i = 0; i < N; ++i) {
        v.push_back(Q.top());
        Q.pop();
      }
      std::sort(w.begin(), w.end());

      if (! std::equal(v.begin(), v.end(), w.begin())) {
        std::cout << std::endl << "heap sorted: ";
        std::copy(v.begin(), v.end(), 
                  std::ostream_iterator<int>(std::cout," "));
        std::cout << std::endl << "correct: ";
        std::copy(w.begin(), w.end(), 
                  std::ostream_iterator<int>(std::cout," "));
        std::cout << std::endl;
        return -1;
      }
    }
  }
  std::cout << "fibonacci heap passed test" << std::endl; 
  return 0;
}
