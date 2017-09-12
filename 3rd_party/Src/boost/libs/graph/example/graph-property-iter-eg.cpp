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
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/property_iter_range.hpp>

int
main()
{
  using namespace boost;
  typedef adjacency_list < listS, vecS, directedS,
    property < vertex_name_t, std::string > >graph_t;
  graph_t g(3);

  const char *vertex_names[] = { "Kubrick", "Clark", "Hal" };
  int i = 0;
  graph_property_iter_range < graph_t, vertex_name_t >::iterator v, v_end;
  for (tie(v, v_end) = get_property_iter_range(g, vertex_name);
       v != v_end; ++v, ++i)
    *v = vertex_names[i];

  tie(v, v_end) = get_property_iter_range(g, vertex_name);
  std::copy(v, v_end, std::ostream_iterator < std::string > (std::cout, " "));
  std::cout << std::endl;
  return 0;
}
