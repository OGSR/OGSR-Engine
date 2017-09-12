//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
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
#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_utility.hpp>


int main()
{
  using namespace boost;
  enum { A, B, C, D, E, F, N };
  const char* name = "ABCDEF";

  // A directed graph
  
  typedef adjacency_matrix<directedS> Graph;
  Graph g(N);
  add_edge(B, C, g);
  add_edge(B, F, g);
  add_edge(C, A, g);
  add_edge(C, C, g);
  add_edge(D, E, g);
  add_edge(E, D, g);
  add_edge(F, A, g);

  std::cout << "vertex set: ";
  print_vertices(g, name);
  std::cout << std::endl;

  std::cout << "edge set: ";
  print_edges(g, name);
  std::cout << std::endl;

  std::cout << "out-edges: " << std::endl;
  print_graph(g, name);
  std::cout << std::endl;

  // An undirected graph

  typedef adjacency_matrix<undirectedS> UGraph;
  UGraph ug(N);
  add_edge(B, C, ug);
  add_edge(B, F, ug);
  add_edge(C, A, ug);
  add_edge(D, E, ug);
  add_edge(F, A, ug);

  std::cout << "vertex set: ";
  print_vertices(ug, name);
  std::cout << std::endl;

  std::cout << "edge set: ";
  print_edges(ug, name);
  std::cout << std::endl;

  std::cout << "incident edges: " << std::endl;
  print_graph(ug, name);
  std::cout << std::endl;
  return 0;
}
