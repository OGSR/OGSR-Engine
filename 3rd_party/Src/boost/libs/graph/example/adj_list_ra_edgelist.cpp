//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
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
#include <boost/graph/adjacency_list.hpp>

int
main()
{
  using namespace boost;
  typedef adjacency_list<vecS, vecS, bidirectionalS, no_property, 
    property<int, edge_weight_t>, no_property, vecS> Graph;

  const std::size_t n = 3;
  typedef std::pair<std::size_t, std::size_t> E;
  E edge_array[] = { E(0,1), E(0,2), E(0,1) };
  const std::size_t m = sizeof(edge_array) / sizeof(E);
  Graph g(edge_array, edge_array + m, n);
  for (std::size_t i = 0; i < m; ++i)
    std::cout << edges(g).first[i] << " ";
  std::cout << std::endl;
  
  return 0;
}
