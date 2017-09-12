// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

//
// This file checks to see if various standard container
// implementations live up to requirements specified in the C++
// standard. As many implementations do not live to the requirements,
// it is not uncommon for this file to fail to compile. The
// BOOST_HIDE_EXPECTED_ERRORS macro is provided here if you want to
// see as much of this file compile as possible.
//

#include <boost/concept_check.hpp>

#include <iterator>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <deque>
#ifndef BOOST_NO_SLIST
#include <slist>
#endif

// Define this macro if you want to hide the expected error, that is,
// error in the various C++ standard library implementations.
//
//#define BOOST_HIDE_EXPECTED_ERRORS

int
main()
{
  using namespace boost;

#if defined(_ITERATOR_) && defined(BOOST_HIDE_EXPECTED_ERRORS)
  // VC++ STL implementation is not standard conformant and
  // fails to pass these concept checks
#else
  typedef std::vector<int> Vector;
  typedef std::deque<int> Deque;
  typedef std::list<int> List;

  // VC++ missing pointer and const_pointer typedefs
  function_requires< Mutable_RandomAccessContainerConcept<Vector> >();
  function_requires< BackInsertionSequenceConcept<Vector> >();

#if !(defined(__GNUC__) && defined(BOOST_HIDE_EXPECTED_ERRORS))
#if !(defined(__sgi) && defined(BOOST_HIDE_EXPECTED_ERRORS))
  // old deque iterator missing n + iter operation
  function_requires< Mutable_RandomAccessContainerConcept<Deque> >();
#endif
  // warnings about signed and unsigned in old deque version
  function_requires< FrontInsertionSequenceConcept<Deque> >();
  function_requires< BackInsertionSequenceConcept<Deque> >();
#endif

  // VC++ missing pointer and const_pointer typedefs
  function_requires< Mutable_ReversibleContainerConcept<List> >();
  function_requires< FrontInsertionSequenceConcept<List> >();
  function_requires< BackInsertionSequenceConcept<List> >();

#ifndef BOOST_NO_SLIST
  typedef BOOST_STD_EXTENSION_NAMESPACE::slist<int> SList;
  function_requires< FrontInsertionSequenceConcept<SList> >();
#endif

  typedef std::set<int> Set;
  typedef std::multiset<int> MultiSet;
  typedef std::map<int,int> Map;
  typedef std::multimap<int,int> MultiMap;

  function_requires< SortedAssociativeContainerConcept<Set> >();
  function_requires< SimpleAssociativeContainerConcept<Set> >();
  function_requires< UniqueAssociativeContainerConcept<Set> >();

  function_requires< SortedAssociativeContainerConcept<MultiSet> >();
  function_requires< SimpleAssociativeContainerConcept<MultiSet> >();
  function_requires< MultipleAssociativeContainerConcept<MultiSet> >();

  function_requires< SortedAssociativeContainerConcept<Map> >();
  function_requires< UniqueAssociativeContainerConcept<Map> >();
  function_requires< PairAssociativeContainerConcept<Map> >();

  function_requires< SortedAssociativeContainerConcept<MultiMap> >();
  function_requires< MultipleAssociativeContainerConcept<MultiMap> >();
  function_requires< PairAssociativeContainerConcept<MultiMap> >();
#endif

  return 0;
}
