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
#include <vector>
#include <list>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace boost
{
  struct edge_component_t
  {
    enum
    { num = 555 };
    typedef edge_property_tag kind;
  }
  edge_component;
}

int
main()
{
  using namespace boost;
  typedef adjacency_list < vecS, vecS, undirectedS,
    no_property, property < edge_component_t, std::size_t > >graph_t;
  typedef graph_traits < graph_t >::vertex_descriptor vertex_t;
  graph_t g(9);
  add_edge(0, 5, g);
  add_edge(0, 1, g);
  add_edge(0, 6, g);
  add_edge(1, 2, g);
  add_edge(1, 3, g);
  add_edge(1, 4, g);
  add_edge(2, 3, g);
  add_edge(4, 5, g);
  add_edge(6, 8, g);
  add_edge(6, 7, g);
  add_edge(7, 8, g);

  std::size_t c = 0;
  std::vector < std::size_t > discover_time(num_vertices(g));
  std::vector < vertex_t > lowpt(num_vertices(g));
  property_map < graph_t, edge_component_t >::type
    component = get(edge_component, g);
  biconnected_components(0, 8, g, component, c, &discover_time[0], &lowpt[0]);

  std::cout << "graph A {\n" << "  node[shape=\"circle\"]\n";

  graph_traits < graph_t >::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    std::cout << source(*ei, g) << " -- " << target(*ei, g)
      << "[label=\"" << component[*ei] << "\"]\n";
  std::cout << "}\n";

  return 0;
}
