//=======================================================================
// Copyright 2002 Indiana University.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
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

#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

struct vertex_info_t { };
struct edge_info_t { };
namespace boost {
  BOOST_INSTALL_PROPERTY(vertex, info);
  BOOST_INSTALL_PROPERTY(edge, info);
};

typedef property<vertex_info_t, double> vertex_properties;
typedef property<edge_info_t, double> edge_properties;

typedef adjacency_list<vecS, vecS, bidirectionalS,
vertex_properties, edge_properties> graph_t;

double& foo_1(graph_t& x)
{
  property_map<graph_t, vertex_info_t>::type pmap
    = get(vertex_info_t(), x);
  return pmap[vertex(0, x)];
}

const double& foo_2(graph_t const & x)
{
  property_map<graph_t, vertex_info_t>::const_type pmap
    = get(vertex_info_t(), x);
  return pmap[vertex(0, x)];
}

double& bar_1(graph_t& x)
{
  property_map<graph_t, edge_info_t>::type pmap
    = get(edge_info_t(), x);
  return pmap[edge(vertex(0, x), vertex(1, x), x).first];
}

const double& bar_2(graph_t const & x)
{
  property_map<graph_t, edge_info_t>::const_type pmap
    = get(edge_info_t(), x);
  return pmap[edge(vertex(0, x), vertex(1, x), x).first];
}
      
int
main()
{
  return 0;
}
