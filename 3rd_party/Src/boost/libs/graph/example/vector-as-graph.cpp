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

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#error The vector_as_graph.hpp header requires partial specialization
#endif

#include <vector>
#include <list>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/graph_utility.hpp>

int
main()
{
  enum
  { r, s, t, u, v, w, x, y, N };
  char name[] = "rstuvwxy";
  typedef std::vector < std::list < int > > Graph;
  Graph g(N);
  g[r].push_back(v);
  g[s].push_back(r);
  g[s].push_back(r);
  g[s].push_back(w);
  g[t].push_back(x);
  g[u].push_back(t);
  g[w].push_back(t);
  g[w].push_back(x);
  g[x].push_back(y);
  g[y].push_back(u);
  boost::print_graph(g, name);
  return 0;
}
