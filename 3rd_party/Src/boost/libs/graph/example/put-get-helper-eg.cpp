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
#include <vector>
#include <string>
#include <boost/property_map.hpp>

#ifdef BOOST_NO_STD_ITERATOR_TRAITS
#error This examples requires a compiler that provides a working std::iterator_traits
#endif


namespace foo
{
  using namespace boost;
    template < class RandomAccessIterator, class IndexMap >
    class iterator_property_map:public boost::put_get_helper <
    typename std::iterator_traits < RandomAccessIterator >::reference,
    iterator_property_map < RandomAccessIterator, IndexMap > >
  {
  public:
    typedef std::ptrdiff_t key_type;
    typedef typename std::iterator_traits < RandomAccessIterator >::value_type
      value_type;
    typedef typename std::iterator_traits < RandomAccessIterator >::reference
      reference;
    typedef boost::lvalue_property_map_tag category;

      iterator_property_map(RandomAccessIterator cc = RandomAccessIterator(),
                            const IndexMap & _id =
                            IndexMap()):iter(cc), index(_id)
    {
    }
    reference operator[] (std::ptrdiff_t v) const
    {
      return *(iter + get(index, v));
    }
  protected:
      RandomAccessIterator iter;
    IndexMap index;
  };

}

int
main()
{
  typedef std::vector < std::string > vec_t;
  typedef foo::iterator_property_map < vec_t::iterator,
    boost::identity_property_map > pmap_t;
  using namespace boost;
  function_requires < Mutable_LvaluePropertyMapConcept < pmap_t, int > >();
  return 0;
}
