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
//
//  Sample output
//  DFS parenthesis:
//  (0(2(3(4(11)4)3)2)0)

#include <boost/config.hpp>
#include <assert.h>
#include <iostream>

#include <vector>
#include <algorithm>
#include <utility>

#include "boost/graph/visitors.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/breadth_first_search.hpp"
#include "boost/graph/depth_first_search.hpp"

using namespace boost;
using namespace std;

struct open_paren : public base_visitor<open_paren> {
  typedef on_discover_vertex event_filter;
  template <class Vertex, class Graph>
  void operator()(Vertex v, Graph&) {
    std::cout << "(" << v;
  }
};
struct close_paren : public base_visitor<close_paren> {
  typedef on_finish_vertex event_filter;
  template <class Vertex, class Graph>
  void operator()(Vertex v, Graph&) {
    std::cout << v << ")";
  }
};


int 
main(int, char*[])
{

  using namespace boost;
  
  typedef adjacency_list<> Graph;
  typedef std::pair<int,int> E;
  E edge_array[] = { E(0, 2),
                E(1, 1), E(1, 3),
                E(2, 1), E(2, 3),
                E(3, 1), E(3, 4),
                E(4, 0), E(4, 1) };  
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  Graph G(5);
  for (std::size_t j = 0; j < sizeof(edge_array) / sizeof(E); ++j)
    add_edge(edge_array[j].first, edge_array[j].second, G);
#else
  Graph G(edge_array, edge_array + sizeof(edge_array)/sizeof(E), 5);
#endif

  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::vertices_size_type size_type;
  
  std::cout << "DFS parenthesis:" << std::endl;
  depth_first_search(G, visitor(make_dfs_visitor(std::make_pair(open_paren(), 
                                                        close_paren()))));
  std::cout << std::endl;
  return 0;
}

