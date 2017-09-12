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
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_archetypes.hpp>

int main(int,char*[])
{
  using namespace boost;

  // Check graph concepts againt their archetypes
  typedef default_constructible_archetype<
    sgi_assignable_archetype< equality_comparable_archetype<> > > Vertex;

  typedef incidence_graph_archetype<Vertex, directed_tag, 
    allow_parallel_edge_tag> Graph1;
  function_requires< IncidenceGraphConcept<Graph1> >();

  typedef adjacency_graph_archetype<Vertex, directed_tag, 
    allow_parallel_edge_tag> Graph2;
  function_requires< AdjacencyGraphConcept<Graph2> >();

  typedef vertex_list_graph_archetype<Vertex, directed_tag, 
    allow_parallel_edge_tag> Graph3;
  function_requires< VertexListGraphConcept<Graph3> >();

  function_requires< ColorValueConcept<color_value_archetype> >();

  typedef incidence_graph_archetype<Vertex, directed_tag, allow_parallel_edge_tag> G;
  typedef property_graph_archetype<G, vertex_color_t, color_value_archetype>
    Graph4;
  function_requires< PropertyGraphConcept<Graph4, Vertex, vertex_color_t> >();

  return 0;
}
