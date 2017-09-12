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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

/*
  Sample output:

  original graph:
  0 --> 3 2 3 
  1 --> 3 
  2 --> 0 
  3 --> 2 

  removing edges (0,3)
  0 --> 2 
  1 --> 3 
  2 --> 0 
  3 --> 2 
  removing edge (0,2) and (3, 2)
  0 --> 
  1 --> 3 
  2 --> 0 
  3 --> 
  
 */

using namespace boost;

typedef adjacency_list<vecS, vecS, directedS> Graph;


int
main()
{
  typedef std::pair<std::size_t,std::size_t> Edge;
  Edge edges[6] = { Edge(0,3), Edge(0,2), Edge(0, 3),
                    Edge(1,3),
                    Edge(2, 0),
                    Edge(3, 2) };

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle iterator constructor
  Graph g(4);
  for (std::size_t j = 0; j < 6; ++j)
    add_edge(edges[j].first, edges[j].second, g);
#else
  Graph g(edges, edges + 6, 4);
#endif

  std::cout << "original graph:" << std::endl;
  print_graph(g, get(vertex_index, g));
  std::cout << std::endl;

  std::cout << "removing edges (0,3)" << std::endl;
  remove_out_edge_if(vertex(0,g), incident_to(vertex(3,g), g), g);
  print_graph(g, get(vertex_index, g));

  std::cout << "removing edge (0,2) and (3, 2)" << std::endl;
  remove_edge_if(incident_to(vertex(2,g), g), g);
  print_graph(g, get(vertex_index, g));

  return 0;
}
