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

#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>

int
main()
{
  using namespace boost;
  typedef adjacency_list < vecS, vecS, bidirectionalS > Graph;

  Graph G(5);
  add_edge(0, 2, G);
  add_edge(1, 1, G);
  add_edge(1, 3, G);
  add_edge(1, 4, G);
  add_edge(2, 1, G);
  add_edge(2, 3, G);
  add_edge(2, 4, G);
  add_edge(3, 1, G);
  add_edge(3, 4, G);
  add_edge(4, 0, G);
  add_edge(4, 1, G);

  std::cout << "original graph:" << std::endl;
  print_graph(G, get(vertex_index, G));


  std::cout << std::endl << "reversed graph:" << std::endl;
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300  // avoid VC++ bug...
  reverse_graph<Graph> R(G);
  print_graph(R, get(vertex_index, G));
#else
  print_graph(make_reverse_graph(G), get(vertex_index, G));
#endif

  return EXIT_SUCCESS;
}
