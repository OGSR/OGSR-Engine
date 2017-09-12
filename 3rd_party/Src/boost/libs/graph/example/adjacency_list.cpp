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
#include <vector>
#include <utility>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map.hpp>

/*
  Sample Output

  graph name: foo
  0  --joe--> 1  
  1  --joe--> 0   --curly--> 2   --dick--> 3  
  2  --curly--> 1   --tom--> 4  
  3  --dick--> 1   --harry--> 4  
  4  --tom--> 2   --harry--> 3  
  (0,1) (1,2) (1,3) (2,4) (3,4) 

  removing edge (1,3): 
  0  --joe--> 1  
  1  --joe--> 0   --curly--> 2  
  2  --curly--> 1   --tom--> 4  
  3  --harry--> 4  
  4  --tom--> 2   --harry--> 3  
  (0,1) (1,2) (2,4) (3,4) 

 */

int main(int , char* [])
{
  using namespace boost;
  using namespace std;

  typedef property<edge_name_t, std::string> EdgeProperty;
  typedef property<vertex_color_t, default_color_type> ColorProperty;
  typedef property<vertex_index_t, std::size_t, ColorProperty> VertexProperty;

  typedef adjacency_list<vecS, listS, undirectedS, 
    VertexProperty, EdgeProperty> Graph;

  const int V = 5;
  Graph g(V);

  property_map<Graph, vertex_index_t>::type id = get(vertex_index, g);

  property_map<Graph, edge_name_t>::type name = get(edge_name, g);

  boost::graph_traits<Graph>::vertex_iterator vi, viend;
  int vnum = 0;

  for (boost::tie(vi,viend) = vertices(g); vi != viend; ++vi)
    id[*vi] = vnum++;

  add_edge(vertex(0, g), vertex(1, g), EdgeProperty("joe"), g);
  add_edge(vertex(1, g), vertex(2, g), EdgeProperty("curly"), g);
  add_edge(vertex(1, g), vertex(3, g), EdgeProperty("dick"), g);
  add_edge(vertex(2, g), vertex(4, g), EdgeProperty("tom"), g);
  add_edge(vertex(3, g), vertex(4, g), EdgeProperty("harry"), g);

  graph_traits<Graph>::vertex_iterator i, end;
  graph_traits<Graph>::out_edge_iterator ei, edge_end;
  for (boost::tie(i,end) = vertices(g); i != end; ++i) {
    cout << id[*i] << " ";
    for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
      cout << " --" << name[*ei] << "--> " << id[target(*ei, g)] << "  ";
    cout << endl;
  }
  print_edges(g, id);

  cout << endl << "removing edge (1,3): " << endl;  
  remove_edge(vertex(1, g), vertex(3, g), g);

  ei = out_edges(vertex(1, g), g).first;
  cout << "removing edge (" << id[source(*ei, g)] 
       << "," << id[target(*ei, g)] << ")" << endl;
  remove_edge(ei, g);

  for(boost::tie(i,end) = vertices(g); i != end; ++i) {
    cout << id[*i] << " ";
    for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
      cout << " --" << name[*ei] << "--> " << id[target(*ei, g)] << "  ";
    cout << endl;
  }

  print_edges(g, id);

  return 0;
}
