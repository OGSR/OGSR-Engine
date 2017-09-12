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
#include <boost/graph/adjacency_list.hpp>

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(BOOST_NO_STD_ALLOCATOR)

template <class Allocator>
struct list_with_allocatorS { };

namespace boost {
  template <class Alloc, class ValueType>
  struct container_gen<list_with_allocatorS<Alloc>, ValueType> {
    typedef typename Alloc::template rebind<ValueType>::other Allocator;
    typedef std::list<ValueType, Allocator> type;
  };
  template <class Alloc>
  struct parallel_edge_traits< list_with_allocatorS<Alloc> > { 
    typedef allow_parallel_edge_tag type;
  };

}

// now you can define a graph using std::list and a specific allocator  
typedef boost::adjacency_list< list_with_allocatorS< std::allocator<int> >,
  boost::vecS, boost::directedS> MyGraph;

int main(int, char*[])
{
  MyGraph g(5);
  
  return 0;
}

#else

int main(int, char*[])
{
  return 0;
}

#endif
