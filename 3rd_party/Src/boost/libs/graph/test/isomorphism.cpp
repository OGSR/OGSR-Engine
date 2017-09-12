// Boost.Graph library isomorphism test

// Copyright (C) 2001 Doug Gregor (gregod@cs.rpi.edu)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.

// For more information, see http://www.boost.org
//
// Revision History:
//
// 29 Nov 2001    Jeremy Siek
//      Changed to use Boost.Random.
// 29 Nov 2001    Doug Gregor
//      Initial checkin.

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <time.h> // clock used without std:: qualifier?
#include <boost/test/test_tools.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/property_map.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

using namespace boost;

template <typename Generator>
struct random_functor {
  random_functor(Generator& g) : g(g) { }
  std::size_t operator()(std::size_t n) {
    boost::uniform_int<boost::mt19937, std::size_t> x(g, 0, n-1);
    return x();
  }
  Generator& g;
};

template<typename Graph1, typename Graph2>
void randomly_permute_graph(const Graph1& g1, Graph2& g2)
{
  // Need a clean graph to start with
  assert(num_vertices(g2) == 0);
  assert(num_edges(g2) == 0);

  typedef typename graph_traits<Graph1>::vertex_descriptor vertex1;
  typedef typename graph_traits<Graph2>::vertex_descriptor vertex2;
  typedef typename graph_traits<Graph1>::edge_iterator edge_iterator;

  boost::mt19937 gen;
  random_functor<boost::mt19937> rand_fun(gen);

  // Decide new order
  std::vector<vertex1> orig_vertices(vertices(g1).first, vertices(g1).second);
  std::random_shuffle(orig_vertices.begin(), orig_vertices.end(), rand_fun);
  std::map<vertex1, vertex2> vertex_map;

  for (std::size_t i = 0; i < num_vertices(g1); ++i) {
    vertex_map[orig_vertices[i]] = add_vertex(g2);
  }

  for (edge_iterator e = edges(g1).first; e != edges(g1).second; ++e) {
    add_edge(vertex_map[source(*e, g1)], vertex_map[target(*e, g1)], g2);
  }
}

template<typename Graph>
void generate_random_digraph(Graph& g, double edge_probability)
{
  typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
  boost::mt19937 random_gen;
  boost::uniform_real<boost::mt19937, double> 
    random_dist(random_gen, 0.0, 1.0);

  for (vertex_iterator u = vertices(g).first; u != vertices(g).second; ++u) {
    vertex_iterator v = u;
    ++v;
    for (; v != vertices(g).second; ++v) {
      if (random_dist() <= edge_probability)
        add_edge(*u, *v, g);
    }
  }
}

void test_isomorphism(int n, double edge_probability) 
{
  typedef adjacency_list<vecS, vecS, bidirectionalS> graph1;
  typedef adjacency_list<listS, listS, bidirectionalS,
                         property<vertex_index_t, int> > graph2;

  graph1 g1(n);
  generate_random_digraph(g1, edge_probability);
  graph2 g2;
  randomly_permute_graph(g1, g2);

  int v_idx = 0;
  for (graph2::vertex_iterator v = vertices(g2).first; 
       v != vertices(g2).second; ++v) {
    put(vertex_index_t(), g2, *v, v_idx++);
  }

  std::map<graph1::vertex_descriptor, graph2::vertex_descriptor> mapping;

  bool isomorphism_correct;
  clock_t start = clock();
  BOOST_TEST(isomorphism_correct = isomorphism
               (g1, g2, isomorphism_map(make_assoc_property_map(mapping))));
  clock_t end = clock();

  std::cout << "Elapsed time (clock cycles): " << (end - start) << std::endl;

  bool verify_correct;
  BOOST_TEST(verify_correct = 
             verify_isomorphism(g1, g2, make_assoc_property_map(mapping)));

  if (!isomorphism_correct || !verify_correct) {
    // Output graph 1
    {
      std::ofstream out("isomorphism_failure.bg1");
      out << num_vertices(g1) << std::endl;
      for (graph1::edge_iterator e = edges(g1).first; 
           e != edges(g1).second; ++e) {
        out << get(vertex_index_t(), g1, source(*e, g1)) << ' '
            << get(vertex_index_t(), g1, target(*e, g1)) << std::endl;
      }
    }

    // Output graph 2
    {
      std::ofstream out("isomorphism_failure.bg2");
      out << num_vertices(g2) << std::endl;
      for (graph2::edge_iterator e = edges(g2).first; 
           e != edges(g2).second; ++e) {
        out << get(vertex_index_t(), g2, source(*e, g2)) << ' '
            << get(vertex_index_t(), g2, target(*e, g2)) << std::endl;
      }
    }
  }
}

int test_main(int argc, char* argv[])
{
  if (argc < 3) {
    test_isomorphism(30, 0.45);
    return 0;
  }

  int n = atoi(argv[1]);
  double edge_prob = atof(argv[2]);
  test_isomorphism(n, edge_prob);

  return 0;
}
