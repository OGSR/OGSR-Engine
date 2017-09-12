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
#include <boost/graph/leda_graph.hpp>
#include <iostream>
#undef string                   // LEDA macro!
int
main()
{
  using namespace boost;
  typedef GRAPH < std::string, int >graph_t;
  graph_t g;
  g.new_node("Philoctetes");
  g.new_node("Heracles");
  g.new_node("Alcmena");
  g.new_node("Eurystheus");
  g.new_node("Amphitryon");
  typedef property_map < graph_t, vertex_all_t >::type NodeMap;
  NodeMap node_name_map = get(vertex_all, g);
  graph_traits < graph_t >::vertex_iterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    std::cout << node_name_map[*vi] << std::endl;
  return EXIT_SUCCESS;
}
