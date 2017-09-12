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
#include <vector>
#include <string>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/leda_graph.hpp>
// Undefine macros from LEDA that conflict with the C++ Standard Library.
#undef string
#undef vector

int
main()
{
  using namespace boost;
  typedef GRAPH < std::string, char >graph_t;
  graph_t leda_g;
  typedef graph_traits < graph_t >::vertex_descriptor vertex_t;
  std::vector < vertex_t > vert(7);
  vert[0] = add_vertex(std::string("pick up kids from school"), leda_g);
  vert[1] = add_vertex(std::string("buy groceries (and snacks)"), leda_g);
  vert[2] = add_vertex(std::string("get cash at ATM"), leda_g);
  vert[3] =
    add_vertex(std::string("drop off kids at soccer practice"), leda_g);
  vert[4] = add_vertex(std::string("cook dinner"), leda_g);
  vert[5] = add_vertex(std::string("pick up kids from soccer"), leda_g);
  vert[6] = add_vertex(std::string("eat dinner"), leda_g);

  add_edge(vert[0], vert[3], leda_g);
  add_edge(vert[1], vert[3], leda_g);
  add_edge(vert[1], vert[4], leda_g);
  add_edge(vert[2], vert[1], leda_g);
  add_edge(vert[3], vert[5], leda_g);
  add_edge(vert[4], vert[6], leda_g);
  add_edge(vert[5], vert[6], leda_g);

  std::vector < vertex_t > topo_order;
  node_array < default_color_type > color_array(leda_g);

  topological_sort(leda_g, std::back_inserter(topo_order),
                   color_map(make_leda_node_property_map(color_array)));

  std::reverse(topo_order.begin(), topo_order.end());
  int n = 1;
  for (std::vector < vertex_t >::iterator i = topo_order.begin();
       i != topo_order.end(); ++i, ++n)
    std::cout << n << ": " << leda_g[*i] << std::endl;

  return EXIT_SUCCESS;
}
