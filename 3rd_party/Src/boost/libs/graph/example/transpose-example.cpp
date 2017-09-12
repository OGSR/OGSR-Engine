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
#include <boost/graph/transpose_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

int
main()
{
  using namespace boost;
  typedef int weight_t;
  typedef adjacency_list < vecS, vecS, bidirectionalS,
    property < vertex_name_t, char > > graph_t;

  enum { a, b, c, d, e, f, g, N };
  graph_t G(N);
  property_map < graph_t, vertex_name_t >::type
    name_map = get(vertex_name, G);
  char name = 'a';
  graph_traits < graph_t >::vertex_iterator v, v_end;
  for (tie(v, v_end) = vertices(G); v != v_end; ++v, ++name)
    name_map[*v] = name;

  typedef std::pair < int, int >E;
  E edge_array[] = { E(a, c), E(a, d), E(b, a), E(b, d), E(c, f),
    E(d, c), E(d, e), E(d, f), E(e, b), E(e, g), E(f, e), E(f, g)
  };
  for (int i = 0; i < 12; ++i)
    add_edge(edge_array[i].first, edge_array[i].second, G);

  print_graph(G, name_map);
  std::cout << std::endl;

  graph_t G_T;
  transpose_graph(G, G_T);

  print_graph(G_T, name_map);

  graph_traits < graph_t >::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    assert(edge(target(*ei, G), source(*ei, G), G_T).second == true);
  return 0;
}
