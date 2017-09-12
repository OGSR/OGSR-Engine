//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Jeremy G. Siek
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

/*
  Sample output:

  G0:
  0 --> 1 
  1 --> 2 3 
  2 --> 5 
  3 --> 
  4 --> 1 5 
  5 --> 3 
  0(0,1) 1(1,2) 2(1,3) 6(2,5) 3(4,1) 4(4,5) 5(5,3) 

  G1:
  2 --> 5 
  4 --> 5 
  5 --> 
  6(2,5) 4(4,5) 

  G2:
  0 --> 1 
  1 --> 
  0(0,1) 

 */

#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

int main(int,char*[])
{
  using namespace boost;
  typedef adjacency_list_traits<vecS, vecS, directedS> Traits;
  typedef subgraph< adjacency_list<vecS, vecS, directedS,
    property<vertex_color_t, int>, property<edge_index_t, int> > > Graph;

  const int N = 6;
  Graph G0(N);
  enum { A, B, C, D, E, F};     // for conveniently refering to vertices in G0

  Graph& G1 = G0.create_subgraph();
  Graph& G2 = G0.create_subgraph();
  enum { A1, B1, C1 };          // for conveniently refering to vertices in G1
  enum { A2, B2 };              // for conveniently refering to vertices in G2

  add_vertex(C, G1); // global vertex C becomes local A1 for G1
  add_vertex(E, G1); // global vertex E becomes local B1 for G1
  add_vertex(F, G1); // global vertex F becomes local C1 for G1
  
  add_vertex(A, G2); // global vertex A becomes local A1 for G2
  add_vertex(B, G2); // global vertex B becomes local B1 for G2

  add_edge(A, B, G0);
  add_edge(B, C, G0);
  add_edge(B, D, G0);
  add_edge(E, B, G0);
  add_edge(E, F, G0);
  add_edge(F, D, G0);

  add_edge(A1, C1, G1); // (A1,C1) is subgraph G1 local indices for (C,F).

  std::cout << "G0:" << std::endl;
  print_graph(G0, get(vertex_index, G0));
  print_edges2(G0, get(vertex_index, G0), get(edge_index, G0));
  std::cout << std::endl;

  Graph::children_iterator ci, ci_end;
  int num = 1;
  for (tie(ci, ci_end) = G0.children(); ci != ci_end; ++ci) {
    std::cout << "G" << num++ << ":" << std::endl;
    print_graph(*ci, get(vertex_index, *ci));
    print_edges2(*ci, get(vertex_index, *ci), get(edge_index, *ci));
    std::cout << std::endl;
  }

  return 0;
}
