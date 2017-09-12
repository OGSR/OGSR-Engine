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

#include <boost/graph/adjacency_list.hpp>

/*
  Sample Output

  0 <-- 
  1 <-- 0  
  2 <-- 1  
  3 <-- 1  
  4 <-- 2  3  

 */

int main(int , char* [])
{
  using namespace boost;
  using namespace std;
  using namespace boost;

  typedef adjacency_list<listS,vecS,bidirectionalS> Graph;
  const int num_vertices = 5;
  Graph g(num_vertices);

  add_edge(0, 1, g);
  add_edge(1, 2, g);
  add_edge(1, 3, g);
  add_edge(2, 4, g);
  add_edge(3, 4, g);

  boost::graph_traits<Graph>::vertex_iterator i, end;
  boost::graph_traits<Graph>::in_edge_iterator ei, edge_end;

  for(tie(i,end) = vertices(g); i != end; ++i) {
    cout << *i << " <-- ";
    for (tie(ei,edge_end) = in_edges(*i, g); ei != edge_end; ++ei)
      cout << source(*ei, g) << "  ";
    cout << endl;
  }
  return 0;
}
