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
#include <iostream>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <map>

using namespace boost;

template < typename DistanceMap > class bacon_number_recorder:public default_bfs_visitor
{
public:
bacon_number_recorder(DistanceMap dist):d(dist) {
  }

  template < typename Edge, typename Graph >
    void tree_edge(Edge e, const Graph & g) const
  {
    typename graph_traits < Graph >::vertex_descriptor
      u = source(e, g), v = target(e, g);
      d[v] = d[u] + 1;
  }
private:
    DistanceMap d;
};

// Convenience function
template < typename DistanceMap >
  bacon_number_recorder < DistanceMap > record_bacon_number(DistanceMap d)
{
  return bacon_number_recorder < DistanceMap > (d);
}


int
main()
{
  std::ifstream datafile("./kevin-bacon.dat");
  if (!datafile) {
    std::cerr << "No ./kevin-bacon.dat file" << std::endl;
    return EXIT_FAILURE;
  }

  typedef adjacency_list < vecS, vecS, undirectedS, property < vertex_name_t,
    std::string >, property < edge_name_t, std::string > > Graph;
  Graph g;

  typedef property_map < Graph, vertex_name_t >::type actor_name_map_t;
  actor_name_map_t actor_name = get(vertex_name, g);
  typedef property_map < Graph, edge_name_t >::type movie_name_map_t;
  movie_name_map_t connecting_movie = get(edge_name, g);

  typedef graph_traits < Graph >::vertex_descriptor Vertex;
  typedef std::map < std::string, Vertex > NameVertexMap;
  NameVertexMap actors;

  for (std::string line; std::getline(datafile, line);) {
    char_delimiters_separator < char >sep(false, "", ";");
    tokenizer <> line_toks(line, sep);
    tokenizer <>::iterator i = line_toks.begin();
    std::string actors_name = *i++;
    NameVertexMap::iterator pos;
    bool inserted;
    Vertex u, v;
    tie(pos, inserted) = actors.insert(std::make_pair(actors_name, Vertex()));
    if (inserted) {
      u = add_vertex(g);
      actor_name[u] = actors_name;
      pos->second = u;
    } else
      u = pos->second;

    std::string movie_name = *i++;

    tie(pos, inserted) = actors.insert(std::make_pair(*i, Vertex()));
    if (inserted) {
      v = add_vertex(g);
      actor_name[v] = *i;
      pos->second = v;
    } else
      v = pos->second;

    graph_traits < Graph >::edge_descriptor e;
    tie(e, inserted) = add_edge(u, v, g);
    if (inserted)
      connecting_movie[e] = movie_name;

  }

  std::vector < int >bacon_number(num_vertices(g));

  Vertex src = actors["Kevin Bacon"];
  bacon_number[src] = 0;

  breadth_first_search(g, src,
                       visitor(record_bacon_number(&bacon_number[0])));

  graph_traits < Graph >::vertex_iterator i, end;
  for (tie(i, end) = vertices(g); i != end; ++i) {
    std::cout << actor_name[*i] << " has a Bacon number of "
      << bacon_number[*i] << std::endl;
  }

  return 0;
}
