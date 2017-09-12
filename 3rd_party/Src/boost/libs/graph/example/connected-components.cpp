//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University,
// Bloomington, IN 47405.
//
// Permission to modify the code and to distribute the code is
// granted, provided the text of this NOTICE is retained, a notice if
// the code was modified is included with the above COPYRIGHT NOTICE
// and with the COPYRIGHT NOTICE in the LICENSE file, and that the
// LICENSE file is distributed with the modified code.
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
#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>

int
main()
{
  using namespace boost;
  typedef adjacency_list < vecS, vecS, undirectedS > Graph;
  typedef graph_traits < Graph >::vertex_descriptor Vertex;

  const int N = 6;
  Graph G(N);
  add_edge(0, 1, G);
  add_edge(1, 4, G);
  add_edge(4, 0, G);
  add_edge(2, 5, G);

  std::vector<int> c(num_vertices(G));
  int num = connected_components
    (G, make_iterator_property_map(c.begin(), get(vertex_index, G), c[0]));

  std::cout << std::endl;
  std::vector < int >::iterator i;
  std::cout << "Total number of components: " << num << std::endl;
  for (i = c.begin(); i != c.end(); ++i)
    std::cout << "Vertex " << i - c.begin()
      << " is in component " << *i << std::endl;
  std::cout << std::endl;
  return EXIT_SUCCESS;
}
