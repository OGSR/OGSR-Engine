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
#include <fstream>
#include <map>
#include <string>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>

int
main()
{
  using namespace boost;
  GraphvizDigraph g;
  read_graphviz("figs/scc.dot", g);

  typedef graph_traits < GraphvizDigraph >::vertex_descriptor vertex_t;
  std::map < vertex_t, int >component;

  int num_comp = strong_components(g, make_assoc_property_map(component));

  property_map < GraphvizDigraph, vertex_attribute_t >::type
    vertex_attr_map = get(vertex_attribute, g);
  std::string color[] = {
  "white", "gray", "black", "lightgray"};
  graph_traits < GraphvizDigraph >::vertex_iterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
    vertex_attr_map[*vi]["color"] = color[component[*vi]];
    vertex_attr_map[*vi]["style"] = "filled";
    if (vertex_attr_map[*vi]["color"] == "black")
      vertex_attr_map[*vi]["fontcolor"] = "white";
  }
  write_graphviz("figs/scc-out.dot", g);

  return EXIT_SUCCESS;
}
