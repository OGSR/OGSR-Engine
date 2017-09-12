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
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>

using namespace std;
using namespace boost;


/*
  Vertex Basics

  This example demonstrates the GGCL Vertex interface.

  Sample output:

  vertices(g) = 0 1 2 3 4 
  vertex id: 0
  out-edges: (0,1) (0,2) (0,3) (0,4) 
  in-edges: (2,0) (3,0) (4,0) 
  adjacent vertices: 1 2 3 4 

  vertex id: 1
  out-edges: 
  in-edges: (0,1) (3,1) (4,1) 
  adjacent vertices: 

  vertex id: 2
  out-edges: (2,0) (2,4) 
  in-edges: (0,2) 
  adjacent vertices: 0 4 

  vertex id: 3
  out-edges: (3,0) (3,1) (3,4) 
  in-edges: (0,3) 
  adjacent vertices: 0 1 4 

  vertex id: 4
  out-edges: (4,0) (4,1) 
  in-edges: (0,4) (2,4) (3,4) 
  adjacent vertices: 0 1 


 */


/* some helper functors for output */

template <class Graph>
struct print_edge {
  print_edge(Graph& g) : G(g) { }

  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  void operator()(Edge e) const
  {
    typename boost::property_map<Graph, vertex_index_t>::type 
      id = get(vertex_index, G);

    Vertex src = source(e, G);
    Vertex targ = target(e, G);

    cout << "(" << id[src] << "," << id[targ] << ") ";
  }

  Graph& G;
};

template <class Graph>
struct print_index {
  print_index(Graph& g) : G(g){ }

  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  void operator()(Vertex c) const
  {
    typename boost::property_map<Graph,vertex_index_t>::type 
      id = get(vertex_index, G);
    cout << id[c] << " ";
  }

  Graph& G;
};


template <class Graph>
struct exercise_vertex {
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

  exercise_vertex(Graph& _g) : g(_g) { }

  void operator()(Vertex v) const
  {
    typename boost::property_map<Graph, vertex_index_t>::type 
      id = get(vertex_index, g);

    cout << "vertex id: " << id[v] << endl;
    
    cout << "out-edges: ";
    for_each(out_edges(v, g).first, out_edges(v,g).second, 
             print_edge<Graph>(g));

    cout << endl;

    cout << "in-edges: ";
    for_each(in_edges(v, g).first, in_edges(v,g).second, 
             print_edge<Graph>(g));

    cout << endl;
    
    cout << "adjacent vertices: ";
    for_each(adjacent_vertices(v,g).first, 
             adjacent_vertices(v,g).second, print_index<Graph>(g));
    cout << endl << endl;
  }

  Graph& g;
};


int
main()
{
  typedef adjacency_list<vecS,vecS,bidirectionalS> MyGraphType;

  typedef pair<int,int> Pair;
  Pair edge_array[11] = { Pair(0,1), Pair(0,2), Pair(0,3), Pair(0,4), 
                          Pair(2,0), Pair(3,0), Pair(2,4), Pair(3,1), 
                          Pair(3,4), Pair(4,0), Pair(4,1) };

  /* Construct a graph using the edge_array*/
  MyGraphType g(5);
  for (int i=0; i<11; ++i)
    add_edge(edge_array[i].first, edge_array[i].second, g);

  boost::property_map<MyGraphType, vertex_index_t>::type 
    id = get(vertex_index, g);

  cout << "vertices(g) = ";
  boost::graph_traits<MyGraphType>::vertex_iterator vi;
  for (vi = vertices(g).first; vi != vertices(g).second; ++vi)
    std::cout << id[*vi] <<  " ";
  std::cout << std::endl;

  /* Use the STL for_each algorithm to "exercise" all
     of the vertices in the graph */
  for_each(vertices(g).first, vertices(g).second,
           exercise_vertex<MyGraphType>(g));

  return 0;
}
