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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/pending/integer_range.hpp>
#include <boost/pending/indirect_cmp.hpp>
using namespace boost;
template < typename TimeMap > class dfs_time_visitor:public default_dfs_visitor {
  typedef typename property_traits < TimeMap >::value_type T;
public:
  dfs_time_visitor(TimeMap dmap, TimeMap fmap, T & t)
:  m_dtimemap(dmap), m_ftimemap(fmap), m_time(t) {
  }
  template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph & g) const
  {
    put(m_dtimemap, u, m_time++);
  }
  template < typename Vertex, typename Graph >
    void finish_vertex(Vertex u, const Graph & g) const
  {
    put(m_ftimemap, u, m_time++);
  }
  TimeMap m_dtimemap;
  TimeMap m_ftimemap;
  T & m_time;
};


int
main()
{
  // Select the graph type we wish to use
  typedef adjacency_list < vecS, vecS, directedS > graph_t;
  typedef graph_traits < graph_t >::vertices_size_type size_type;
  // Set up the vertex names
  enum
  { u, v, w, x, y, z, N };
  char name[] = { 'u', 'v', 'w', 'x', 'y', 'z' };
  // Specify the edges in the graph
  typedef std::pair < int, int >E;
  E edge_array[] = { E(u, v), E(u, x), E(x, v), E(y, x),
    E(v, y), E(w, y), E(w, z), E(z, z)
  };
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  graph_t g(N);  
  for (std::size_t j = 0; j < sizeof(edge_array) / sizeof(E); ++j)
    add_edge(edge_array[j].first, edge_array[j].second, g);
#else
  graph_t g(edge_array, edge_array + sizeof(edge_array) / sizeof(E), N);
#endif

  // Typedefs
  typedef boost::graph_traits < graph_t >::vertex_descriptor Vertex;
  typedef size_type* Iiter;

  // discover time and finish time properties
  std::vector < size_type > dtime(num_vertices(g));
  std::vector < size_type > ftime(num_vertices(g));
  size_type t = 0;
  dfs_time_visitor < size_type * >vis(&dtime[0], &ftime[0], t);

  depth_first_search(g, visitor(vis));

  // use std::sort to order the vertices by their discover time
  std::vector < size_type > discover_order(N);
  integer_range < size_type > r(0, N);
  std::copy(r.begin(), r.end(), discover_order.begin());
  std::sort(discover_order.begin(), discover_order.end(),
            indirect_cmp < Iiter, std::less < size_type > >(&dtime[0]));
  std::cout << "order of discovery: ";
  int i;
  for (i = 0; i < N; ++i)
    std::cout << name[discover_order[i]] << " ";

  std::vector < size_type > finish_order(N);
  std::copy(r.begin(), r.end(), finish_order.begin());
  std::sort(finish_order.begin(), finish_order.end(),
            indirect_cmp < Iiter, std::less < size_type > >(&ftime[0]));
  std::cout << std::endl << "order of finish: ";
  for (i = 0; i < N; ++i)
    std::cout << name[finish_order[i]] << " ";
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
