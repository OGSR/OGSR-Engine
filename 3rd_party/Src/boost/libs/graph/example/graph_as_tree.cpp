//=======================================================================
// Copyright 2002 Indiana University.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
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

#include <boost/graph/graph_as_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/cstdlib.hpp>

class tree_printer {
public:
  template <typename Node, typename Tree> 
  void preorder(Node, Tree&) {
    std::cout << "(";
  }
  template <typename Node, typename Tree> 
  void inorder(Node n, Tree& t)
  {
    std::cout << get(boost::vertex_name, t)[n];
  }
  template <typename Node, typename Tree> 
  void postorder(Node, Tree&) {
    std::cout << ")";
  }
  
};

int main()
{
  using namespace boost;
  typedef adjacency_list<vecS, vecS, directedS, 
    property<vertex_name_t, std::string> > graph_t;
  typedef graph_traits<graph_t>::vertex_descriptor vertex_t;

  graph_t g;

  vertex_t a = add_vertex(g),
    b = add_vertex(g),
    c = add_vertex(g);

  add_edge(a, b, g);
  add_edge(a, c, g);
  
  typedef property_map<graph_t, vertex_name_t>::type vertex_name_map_t;
  vertex_name_map_t name = get(vertex_name, g);
  name[a] = "A";
  name[b] = "B";
  name[c] = "C";

  typedef iterator_property_map<std::vector<vertex_t>::iterator,
    property_map<graph_t, vertex_index_t>::type> parent_map_t;
  std::vector<vertex_t> parent(num_vertices(g));
  typedef graph_as_tree<graph_t, parent_map_t> tree_t;
  tree_t t(g, a, make_iterator_property_map(parent.begin(), 
                                            get(vertex_index, g)));

  tree_printer vis;
  traverse_tree(a, t, vis);
  
  return exit_success;
}
