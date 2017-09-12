//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University,
// Bloomington, IN 47405.
//
// Permission to modify the code and to distribute the code is
// granted, provided the text of this NOTICE is retained, a notice if
// the code was modified is included with the above COPYRIGHT NOTICE
// and with the COPYRIGHT NOTICE in the LICENSE file, and that the
// LICENSE file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================
#include <boost/config.hpp>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

template < typename Graph > void
read_graph_file(std::istream & in, Graph & g)
{
  typedef typename graph_traits < Graph >::vertex_descriptor Vertex;
  typedef typename graph_traits < Graph >::vertices_size_type size_type;
  size_type n_vertices;
  in >> n_vertices;             // read in number of vertices
  std::vector < Vertex > vertex_set(n_vertices);
  for (size_type i = 0; i < n_vertices; ++i)
    vertex_set[i] = add_vertex(g);

  size_type u, v;
  while (in >> u)
    if (in >> v)
      add_edge(vertex_set[u], vertex_set[v], g);
    else
      break;
}


int
main()
{
  typedef adjacency_list < listS,       // Store out-edges of each vertex in a std::list
    vecS,                       // Store vertex set in a std::vector
    directedS                   // The graph is directed
  > graph_type;

  graph_type g;                 // use default constructor to create empty graph
  std::ifstream file_in("makefile-dependencies.dat");
  read_graph_file(file_in, g);

  assert(num_vertices(g) == 15);
  assert(num_edges(g) == 19);
  return 0;
}
