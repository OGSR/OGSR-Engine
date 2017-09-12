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
#include <vector>
#include <string>
#include <iostream>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/stanford_graph.hpp>

int
main()
{
  using namespace boost;
  const int n_vertices = 7;
  Graph *sgb_g = gb_new_graph(n_vertices);

  const char *tasks[] = {
    "pick up kids from school",
    "buy groceries (and snacks)",
    "get cash at ATM",
    "drop off kids at soccer practice",
    "cook dinner",
    "pick up kids from soccer",
    "eat dinner"
  };
  const int n_tasks = sizeof(tasks) / sizeof(char *);

  gb_new_arc(sgb_g->vertices + 0, sgb_g->vertices + 3, 0);
  gb_new_arc(sgb_g->vertices + 1, sgb_g->vertices + 3, 0);
  gb_new_arc(sgb_g->vertices + 1, sgb_g->vertices + 4, 0);
  gb_new_arc(sgb_g->vertices + 2, sgb_g->vertices + 1, 0);
  gb_new_arc(sgb_g->vertices + 3, sgb_g->vertices + 5, 0);
  gb_new_arc(sgb_g->vertices + 4, sgb_g->vertices + 6, 0);
  gb_new_arc(sgb_g->vertices + 5, sgb_g->vertices + 6, 0);

  typedef graph_traits < Graph * >::vertex_descriptor vertex_t;
  std::vector < vertex_t > topo_order;
  topological_sort(sgb_g, std::back_inserter(topo_order),
                   vertex_index_map(get(vertex_index, sgb_g)));
  int n = 1;
  for (std::vector < vertex_t >::reverse_iterator i = topo_order.rbegin();
       i != topo_order.rend(); ++i, ++n)
    std::cout << n << ": " << tasks[get(vertex_index, sgb_g)[*i]] << std::endl;

  gb_recycle(sgb_g);
  return EXIT_SUCCESS;
}
