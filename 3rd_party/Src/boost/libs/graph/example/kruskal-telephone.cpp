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
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

int
main()
{
  using namespace boost;
  GraphvizGraph g_dot;
  read_graphviz("figs/telephone-network.dot", g_dot);

  typedef adjacency_list < vecS, vecS, undirectedS, no_property,
    property < edge_weight_t, int > > Graph;
  Graph g(num_vertices(g_dot));
  property_map < GraphvizGraph, edge_attribute_t >::type
    edge_attr_map = get(edge_attribute, g_dot);
  graph_traits < GraphvizGraph >::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(g_dot); ei != ei_end; ++ei) {
    int weight = lexical_cast < int >(edge_attr_map[*ei]["label"]);
    property < edge_weight_t, int >edge_property(weight);
    add_edge(source(*ei, g_dot), target(*ei, g_dot), edge_property, g);
  }

  std::vector < graph_traits < Graph >::edge_descriptor > mst;
  kruskal_minimum_spanning_tree(g, std::back_inserter(mst));

  property_map < Graph, edge_weight_t >::type weight = get(edge_weight, g);
  int total_weight = 0;
  for (int e = 0; e < mst.size(); ++e)
    total_weight += get(weight, mst[e]);
  std::cout << "total weight: " << total_weight << std::endl;

  typedef graph_traits < Graph >::vertex_descriptor Vertex;
  for (int i = 0; i < mst.size(); ++i) {
    Vertex u = source(mst[i], g), v = target(mst[i], g);
    edge_attr_map[edge(u, v, g_dot).first]["color"] = "black";
  }
  std::ofstream out("figs/telephone-mst-kruskal.dot");
  graph_property < GraphvizGraph, graph_edge_attribute_t >::type &
    graph_edge_attr_map = get_property(g_dot, graph_edge_attribute);
  graph_edge_attr_map["color"] = "gray";
  graph_edge_attr_map["style"] = "bold";
  write_graphviz(out, g_dot);

  return EXIT_SUCCESS;
}
