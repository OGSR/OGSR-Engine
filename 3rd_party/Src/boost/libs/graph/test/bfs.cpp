//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Andrew Janiszewski, Jeremy G. Siek
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

#include <boost/test/test_tools.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_archetypes.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <boost/random/mersenne_twister.hpp>

template <typename DistanceMap, typename ParentMap, 
          typename Graph, typename ColorMap>
class bfs_testing_visitor
{
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  typedef typename boost::color_traits<
    typename boost::property_traits<ColorMap>::value_type
  > Color;
public:

  bfs_testing_visitor(Vertex s, DistanceMap d, ParentMap p, ColorMap c)
    : current_distance(0), distance(d), parent(p), color(c), src(s) { }

  void initialize_vertex(const Vertex& u, const Graph& ) const {
    BOOST_TEST(get(color, u) == Color::white());
  }
  void examine_vertex(const Vertex& u, const Graph& ) const {
    current_vertex = u;
    // Ensure that the distances monotonically increase.
    BOOST_TEST( distance[u] == current_distance
                       || distance[u] == current_distance + 1 );
    if (distance[u] == current_distance + 1) // new level
      ++current_distance;
  }
  void discover_vertex(const Vertex& u, const Graph& ) const {
    BOOST_TEST( get(color, u) == Color::gray() );
    if (u == src) {
      current_vertex = src;
    } else {
      BOOST_TEST( parent[u] == current_vertex );
      BOOST_TEST( distance[u] == current_distance + 1 );
      BOOST_TEST( distance[u] == distance[parent[u]] + 1 );
    }
  }
  void examine_edge(const Edge& e, const Graph& g) const {
    BOOST_TEST( source(e, g) == current_vertex );
  }
  void tree_edge(const Edge& e, const Graph& g) const {
    BOOST_TEST( get(color, target(e, g)) == Color::white() );
    Vertex u = source(e, g), v = target(e, g);
    BOOST_TEST( distance[u] == current_distance );
    parent[v] = u;
    distance[v] = distance[u] + 1;
  }
  void non_tree_edge(const Edge& e, const Graph& g) const {
    BOOST_TEST( color[target(e, g)] != Color::white() );

    if (boost::is_directed(g))
      // cross or back edge
      BOOST_TEST(distance[target(e, g)] <= distance[source(e, g)] + 1);
    else {
      // cross edge (or going backwards on a tree edge)
      BOOST_TEST(distance[target(e, g)] == distance[source(e, g)] 
                        || distance[target(e, g)] == distance[source(e, g)] + 1
                        || distance[target(e, g)] == distance[source(e, g)] - 1
                        );
    }
  }

  void gray_target(const Edge& e, const Graph& g) const {
    BOOST_TEST( color[target(e, g)] == Color::gray() );
  }
  
  void black_target(const Edge& e, const Graph& g) const {
    BOOST_TEST( color[target(e, g)] == Color::black() );

    // All vertices adjacent to a black vertex must already be discovered
    typename boost::graph_traits<Graph>::adjacency_iterator ai, ai_end;
    for (boost::tie(ai, ai_end) = adjacent_vertices(target(e, g), g); 
         ai != ai_end; ++ai)
      BOOST_TEST( color[*ai] != Color::white() );
  }
  void finish_vertex(const Vertex& u, const Graph& ) const {
    BOOST_TEST( color[u] == Color::black() );
    
  }
private:
  mutable Vertex current_vertex;
  mutable typename boost::property_traits<DistanceMap>::value_type 
    current_distance;
  DistanceMap distance;
  ParentMap parent;
  ColorMap color;
  Vertex src;
};


template <class Graph>
struct bfs_test
{
  typedef boost::graph_traits<Graph> Traits;
  typedef typename Traits::vertices_size_type
    vertices_size_type;
  static void go(vertices_size_type max_V) {
    typedef typename Traits::vertex_descriptor vertex_descriptor;
    typedef boost::color_traits<boost::default_color_type> Color;

    vertices_size_type i;
    typename Traits::edges_size_type j;
    typename Traits::vertex_iterator ui, ui_end;

    boost::mt19937 gen;

    for (i = 0; i < max_V; ++i)
      for (j = 0; j < i*i; ++j) {
        Graph g;
        generate_random_graph(g, i, j, gen);

        // declare the "start" variable
        vertex_descriptor start = boost::random_vertex(g, gen);

        // vertex properties
        std::vector<int> distance(i, std::numeric_limits<int>::max());
        distance[start] = 0;
        std::vector<vertex_descriptor> parent(i);
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
          parent[*ui] = *ui;
        std::vector<boost::default_color_type> color(i);

        // Create the testing visitor.
        bfs_testing_visitor<int*,vertex_descriptor*,Graph,
          boost::default_color_type*> 
          vis(start, &distance[0], &parent[0], &color[0]);

        boost::breadth_first_search(g, start, 
                                    visitor(vis).
                                    color_map(&color[0]));
        
        // All white vertices should be unreachable from the source.
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
          if (color[*ui] == Color::white()) {
            std::vector<boost::default_color_type> color2(i, Color::white());
            BOOST_TEST(!boost::is_reachable(start, *ui, g, &color2[0]));
          }

        // The shortest path to a child should be one longer than
        // shortest path to the parent.
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
          if (parent[*ui] != *ui) // *ui not the root of the bfs tree
            BOOST_TEST(distance[*ui] == distance[parent[*ui]] + 1);
      }
  }
};


int test_main(int argc, char* argv[])
{
  using namespace boost;
  int max_V = 15;
  if (argc > 1)
    max_V = atoi(argv[1]);

  bfs_test< adjacency_list<vecS, vecS, directedS> >::go(max_V);
  bfs_test< adjacency_list<vecS, vecS, undirectedS> >::go(max_V);
  return 0;
}
