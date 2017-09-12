// 
// concept_checks.cpp -
//   make sure the types meet concept requirements
//

#include "boost/concept_check.hpp"
#include "boost/multi_array/concept_checks.hpp"
#include "boost/multi_array.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "boost/array.hpp"

int
test_main(int,char*[])
{
  const int ndims=3;
  typedef boost::multi_array<int,ndims> array;
  typedef boost::multi_array_ref<int,ndims> array_ref;
  typedef boost::const_multi_array_ref<int,ndims> const_array_ref;
  typedef array::array_view<ndims>::type array_view;
  typedef array::const_array_view<ndims>::type const_array_view;
  typedef array::subarray<ndims>::type subarray;
  typedef array::const_subarray<ndims>::type const_subarray;

boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<subarray,ndims> >();
boost::function_requires<
  boost::detail::multi_array::ConstMultiArrayConcept<const_subarray,ndims> >();

boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array_ref,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<array_view,ndims> >();
boost::function_requires<
  boost::detail::multi_array::MutableMultiArrayConcept<subarray,ndims> >();
 
  return boost::exit_success;
}
