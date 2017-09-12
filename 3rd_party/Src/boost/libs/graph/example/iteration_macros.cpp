//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
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


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

enum family { Jeanie, Debbie, Rick, John, Amanda, Margaret, Benjamin, N };

int main()
{
  using namespace boost;
  const char *name[] = { "Jeanie", "Debbie", "Rick", "John", "Amanda",
    "Margaret", "Benjamin"
  };

  adjacency_list <> g(N);
  add_edge(Jeanie, Debbie, g);
  add_edge(Jeanie, Rick, g);
  add_edge(Jeanie, John, g);
  add_edge(Debbie, Amanda, g);
  add_edge(Rick, Margaret, g);
  add_edge(John, Benjamin, g);

  graph_traits<adjacency_list <> >::vertex_iterator i, end;
  graph_traits<adjacency_list <> >::adjacency_iterator ai, a_end;
  property_map<adjacency_list <>, vertex_index_t>::type
    index_map = get(vertex_index, g);

  BGL_FORALL_VERTICES(i, g, adjacency_list<>) {
    std::cout << name[get(index_map, i)];

    if (out_degree(i, g) == 0)
      std::cout << " has no children";
    else
      std::cout << " is the parent of ";

    BGL_FORALL_ADJACENT(i, j, g, adjacency_list<>)
      std::cout << name[get(index_map, j)] << ", ";
    std::cout << std::endl;
  }
  return EXIT_SUCCESS;
}
