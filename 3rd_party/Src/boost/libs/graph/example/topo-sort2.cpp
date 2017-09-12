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
#include <vector>
#include <deque>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/adjacency_list.hpp>
int
main()
{
  using namespace boost;
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

  adjacency_list < listS, vecS, directedS > g(n_tasks);

  add_edge(0, 3, g);
  add_edge(1, 3, g);
  add_edge(1, 4, g);
  add_edge(2, 1, g);
  add_edge(3, 5, g);
  add_edge(4, 6, g);
  add_edge(5, 6, g);

  std::deque < int >topo_order;

  topological_sort(g, std::front_inserter(topo_order),
                   vertex_index_map(identity_property_map()));

  int n = 1;
  for (std::deque < int >::iterator i = topo_order.begin();
       i != topo_order.end(); ++i, ++n)
    std::cout << tasks[*i] << std::endl;

  return EXIT_SUCCESS;
}
