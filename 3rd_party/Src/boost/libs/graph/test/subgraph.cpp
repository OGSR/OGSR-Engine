//=======================================================================
// Copyright 2001 Indiana University
// Author: Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University, Bloomington,
// IN 47404.
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


#include <set>

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/graph_test.hpp>
#include <boost/graph/iteration_macros.hpp>

#include <boost/random/mersenne_twister.hpp>

// UNDER CONSTRUCTION 

int test_main(int argc, char* argv[])
{
  using namespace boost;
  typedef adjacency_list<vecS, vecS, bidirectionalS,
    no_property, property<edge_index_t, std::size_t> > graph_t;
  typedef subgraph<graph_t> subgraph_t;
  typedef graph_traits<subgraph_t>::vertex_descriptor vertex_t;
  typedef graph_traits<subgraph_t>::edge_descriptor edge_t;

  mt19937 gen;
  for (int t = 0; t < 100; t += 5) {
    subgraph_t g;
    int N = t + 2;
    std::vector<vertex_t> vertex_set;
    std::vector< std::pair<vertex_t, vertex_t> > edge_set;
    generate_random_graph(g, N, N * 2, gen, 
                          std::back_inserter(vertex_set),
                          std::back_inserter(edge_set));

    graph_test< subgraph_t > gt;

    gt.test_incidence_graph(vertex_set, edge_set, g);
    gt.test_bidirectional_graph(vertex_set, edge_set, g);
    gt.test_adjacency_graph(vertex_set, edge_set, g);
    gt.test_vertex_list_graph(vertex_set, g);
    gt.test_edge_list_graph(vertex_set, edge_set, g);
    gt.test_adjacency_matrix(vertex_set, edge_set, g);

    std::vector<vertex_t> sub_vertex_set;
    std::vector<vertex_t> sub_global_map;
    std::vector<vertex_t> global_sub_map(num_vertices(g));
    std::vector< std::pair<vertex_t, vertex_t> > sub_edge_set;

    subgraph_t& g_s = g.create_subgraph();
    
    const std::set<vertex_t>::size_type Nsub = N/2;

    // Collect a set of random vertices to put in the subgraph
    std::set<vertex_t> verts;
    while (verts.size() < Nsub)
      verts.insert(random_vertex(g, gen));

    for (std::set<vertex_t>::iterator i = verts.begin();
        i != verts.end(); ++i) {
      vertex_t v_global = *i;
      vertex_t v = add_vertex(v_global, g_s);
      sub_vertex_set.push_back(v);
      sub_global_map.push_back(v_global);
      global_sub_map[v_global] = v;
    }

    // compute induced edges
    BGL_FORALL_EDGES(e, g, subgraph_t)
      if (contains(sub_global_map, source(e, g))
          && contains(sub_global_map, target(e, g)))
        sub_edge_set.push_back(std::make_pair(global_sub_map[source(e, g)],
                                              global_sub_map[target(e, g)]));

    gt.test_incidence_graph(sub_vertex_set, sub_edge_set, g_s);
    gt.test_bidirectional_graph(sub_vertex_set, sub_edge_set, g_s);
    gt.test_adjacency_graph(sub_vertex_set, sub_edge_set, g_s);
    gt.test_vertex_list_graph(sub_vertex_set, g_s);
    gt.test_edge_list_graph(sub_vertex_set, sub_edge_set, g_s);
    gt.test_adjacency_matrix(sub_vertex_set, sub_edge_set, g_s);
  }  
  return 0;
}
