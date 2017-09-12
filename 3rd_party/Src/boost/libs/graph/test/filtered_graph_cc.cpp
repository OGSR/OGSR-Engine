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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

int main(int,char*[])
{
  using namespace boost;
  // Check filtered_graph
  {
    typedef adjacency_list<vecS, vecS, directedS, 
      no_property, property<edge_residual_capacity_t, long> > Graph;
    typedef property_map<Graph, edge_residual_capacity_t>::type ResCapMap;
    typedef filtered_graph<Graph, is_residual_edge<ResCapMap> > ResGraph;
    typedef graph_traits<ResGraph>::edge_descriptor Edge;

    function_requires< VertexListGraphConcept<ResGraph> >();
    function_requires< EdgeListGraphConcept<ResGraph> >();
    function_requires< IncidenceGraphConcept<ResGraph> >();
    function_requires< AdjacencyGraphConcept<ResGraph> >();
    function_requires< PropertyGraphConcept<ResGraph, Edge, 
      edge_residual_capacity_t> >();
  }
  // Check filtered_graph with bidirectional adjacency_list
  {
    typedef adjacency_list<vecS, vecS, bidirectionalS, 
      no_property, property<edge_residual_capacity_t, long> > Graph;
    typedef property_map<Graph, edge_residual_capacity_t>::type ResCapMap;
    typedef filtered_graph<Graph, is_residual_edge<ResCapMap> > ResGraph;
    function_requires< BidirectionalGraphConcept<ResGraph> >();
  }
  return 0;
}
