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
#include <list>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <iterator>
#include <utility>


typedef std::pair<std::size_t,std::size_t> Pair;

/*
  Topological sort example

  The topological sort algorithm creates a linear ordering
  of the vertices such that if edge (u,v) appears in the graph,
  then u comes before v in the ordering.

  Sample output:

  A topological ordering: 2 5 0 1 4 3

*/

int
main(int , char* [])
{
  //begin
  using namespace boost;

  /* Topological sort will need to color the graph.  Here we use an
     internal decorator, so we "property" the color to the graph.
     */
  typedef adjacency_list<vecS, vecS, directedS, 
    property<vertex_color_t, default_color_type> > Graph;

  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  Pair edges[6] = { Pair(0,1), Pair(2,4),
                    Pair(2,5),
                    Pair(0,3), Pair(1,4),
                    Pair(4,3) };
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle the iterator constructor
  Graph G(6);
  for (std::size_t j = 0; j < 6; ++j)
    add_edge(edges[j].first, edges[j].second, G);
#else
  Graph G(edges, edges + 6, 6);
#endif

  boost::property_map<Graph, vertex_index_t>::type id = get(vertex_index, G);

  typedef std::vector< Vertex > container;
  container c;
  topological_sort(G, std::back_inserter(c));

  std::cout << "A topological ordering: ";
  for (container::reverse_iterator ii = c.rbegin(); 
       ii != c.rend(); ++ii)
    std::cout << id[*ii] << " ";
  std::cout << std::endl;

  return 0;
} 

