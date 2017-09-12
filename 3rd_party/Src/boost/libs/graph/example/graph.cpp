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
#include <algorithm>

#include <boost/graph/adjacency_list.hpp>

using namespace boost;
using namespace std;

typedef property<vertex_color_t, default_color_type,
    property<vertex_distance_t,int,
      property<vertex_degree_t,int,
        property<vertex_in_degree_t, int,
          property<vertex_out_degree_t,int> > > > > VertexProperty;
typedef property<edge_weight_t,int> EdgeProperty;
typedef adjacency_list<vecS, vecS, bidirectionalS, 
                       VertexProperty, EdgeProperty> Graph;

template <class Graph>
void print(Graph& g) {
  typename Graph::vertex_iterator i, end;
  typename Graph::out_edge_iterator ei, edge_end;
  for(boost::tie(i,end) = vertices(g); i != end; ++i) {
    cout << *i << " --> ";
    for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
      cout << target(*ei, g) << "  ";
    cout << endl;
  }
}

std::size_t myrand(std::size_t N) {
  std::size_t ret = rand() % N; 
  //  cout << "N = " << N << "  rand = " << ret << endl;
  return ret;
}

template <class Graph>
bool check_edge(Graph& g, std::size_t a, std::size_t b) {
  typedef typename Graph::vertex_descriptor Vertex;
  typename Graph::adjacency_iterator vi, viend, found;
  boost::tie(vi, viend) = adjacent_vertices(vertex(a,g), g);

  found = find(vi, viend, vertex(b, g));
  if ( found == viend )
    return false;

  return true;
}

int main(int, char*[])
{
  std::size_t N = 5;

  Graph g(N);
  int i;

  bool is_failed = false;

  for (i=0; i<6; ++i) {
    std::size_t a = myrand(N), b = myrand(N);
    while ( a == b ) b = myrand(N);
    cout << "edge edge (" << a << "," << b <<")" << endl;
    //add edges
    add_edge(a, b, g);
    is_failed =  is_failed || (! check_edge(g, a, b) );
  }
  
  if ( is_failed )
    cerr << "    Failed."<< endl;
  else
    cerr << "           Passed."<< endl;
  
  print(g);
  
  //remove_edge
  for (i = 0; i<2; ++i) {
    std::size_t a = myrand(N), b = myrand(N);
    while ( a == b ) b = myrand(N);
    cout << "remove edge (" << a << "," << b <<")" << endl;
    remove_edge(a, b, g);
    is_failed = is_failed || check_edge(g, a, b);
  }
  if ( is_failed )
    cerr << "    Failed."<< endl;
  else
    cerr << "           Passed."<< endl;

  print(g);
  
  //add_vertex
  is_failed = false;
  std::size_t old_N = N;
  std::size_t vid   = add_vertex(g);
  std::size_t vidp1 = add_vertex(g);
  
  N = num_vertices(g);
  if ( (N - 2) != old_N )
    cerr << "    Failed."<< endl;
  else
    cerr << "           Passed."<< endl;      
  
  is_failed = false;
  for (i=0; i<2; ++i) {
    std::size_t a = myrand(N), b = myrand(N);
    while ( a == vid ) a = myrand(N);
    while ( b == vidp1 ) b = myrand(N);
    cout << "add edge (" << vid << "," << a <<")" << endl;
    cout << "add edge (" << vid << "," << vidp1 <<")" << endl;
    add_edge(vid, a, g);
    add_edge(b, vidp1, g);
    is_failed = is_failed || ! check_edge(g, vid, a);
    is_failed = is_failed || ! check_edge(g, b, vidp1);
  }
  if ( is_failed )
    cerr << "    Failed."<< endl;
  else
    cerr << "           Passed."<< endl;
  print(g);
  
  // clear_vertex
  std::size_t c = myrand(N);
  is_failed = false;
  clear_vertex(c, g);

  if ( out_degree(c, g) != 0 )
    is_failed = true;

  cout << "Removing vertex " << c << endl;
  remove_vertex(c, g);
  
  old_N = N;
  N = num_vertices(g);
  
  if ( (N + 1) != old_N )
    is_failed = true;
  
  if ( is_failed )
    cerr << "    Failed."<< endl;
  else
    cerr << "           Passed."<< endl;      
  
  print(g);
  
  return 0;
}
