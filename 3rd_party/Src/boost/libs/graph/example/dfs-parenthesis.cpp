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
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>

char name[] = "abcdefghij";

struct parenthesis_visitor : public boost::default_dfs_visitor
{
  template <class Vertex, class Graph> void
  start_vertex(Vertex v, const Graph &)
  {
    std::cout << ' ';
  }
  template <class Vertex, class Graph> void
  discover_vertex(Vertex v, const Graph &)
  {
    std::cout << "(" << name[v] << ' ';
  }
  template <class Vertex, class Graph> void
  finish_vertex(Vertex v, const Graph &)
  {
    std::cout << ' ' << name[v] << ")";
  }
};

int
main()
{
  using namespace boost;
  GraphvizGraph g;
  read_graphviz("figs/dfs-example.dot", g);
  graph_traits < GraphvizGraph >::edge_iterator e, e_end;
  for (tie(e, e_end) = edges(g); e != e_end; ++e)
    std::cout << '(' << name[source(*e, g)] << ' '
      << name[target(*e, g)] << ')' << std::endl;
  parenthesis_visitor
    paren_vis;
  depth_first_search(g, visitor(paren_vis));
  std::cout << std::endl;
  return 0;
}
