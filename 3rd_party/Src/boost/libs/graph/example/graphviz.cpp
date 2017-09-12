//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Lie-Quan Lee
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
#include <fstream>
#include <string>
#include <map>

#include <boost/graph/graphviz.hpp>

using boost::GraphvizDigraph;

template <class Vertex>
const std::string& 
vertex_label(const Vertex& u, const GraphvizDigraph& g) {
  boost::property_map<GraphvizDigraph, boost::vertex_attribute_t>::const_type
    va = boost::get(boost::vertex_attribute, g); 
  return (*(va[u].find("label"))).second;
}


void print(GraphvizDigraph& g) {
  typedef GraphvizDigraph Graph;
  boost::graph_traits<Graph>::vertex_iterator i, end;
  boost::graph_traits<Graph>::out_edge_iterator ei, edge_end;
  for(boost::tie(i,end) = boost::vertices(g); i != end; ++i) {
    std::cout << vertex_label(*i, g) << " --> ";
    for (boost::tie(ei,edge_end) = boost::out_edges(*i, g); 
         ei != edge_end; ++ei)
      std::cout << vertex_label(boost::target(*ei, g), g) << "  ";
    std::cout << std::endl;
  }
}

/*
  This is to give an example of read graphviz file and create BGL graph.
  Then write the graph to the graphviz file.
*/


int main(int argc, char* argv[]) {
  std::cout << "This is an example to demonstrate how to read graphviz file"
            << std::endl 
            << "and how to write graph to graphviz format."
            << std::endl 
            << std::endl 
            << std::endl;

  std::cout << "Usage: " << argv[0] << "  <input>.dot  <output>.dot " 
            << std::endl;
  std::cout << "If only have one xxx.dot in command line," << std::endl 
            << "the second dot is graphviz_test_new.dot by default." << std::endl
            << "If there is no input and output dot file in command line, "
            << "input is graphviz_test.dot and output graphviz_test_new.dot." << std::endl;

  std::string filename = "graphviz_test.dot";

  if ( argc > 1 )
    filename = argv[1];
  
  GraphvizDigraph g;

  boost::read_graphviz(filename, g);

  print(g);

  const char* dot = "graphviz_test_new.dot";
  if ( argc > 2 ) 
    dot = argv[2];

  boost::write_graphviz(dot, g);
  
  return 0;
}
