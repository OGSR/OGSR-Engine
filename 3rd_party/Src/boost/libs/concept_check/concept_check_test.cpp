// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/concept_check.hpp>
#include <boost/concept_archetype.hpp>

/*

  This file verifies that function_requires() of the Boost Concept
  Checking Library does not cause errors when it is not suppose to
  and verifies that the concept archetypes meet the requirements of
  their matching concepts.

*/


int
main()
{
  using namespace boost;

  //===========================================================================
  // Basic Concepts
  {
    typedef default_constructible_archetype<> foo;
    function_requires< DefaultConstructibleConcept<foo> >();
  }
  {
    typedef assignable_archetype<> foo;
    function_requires< AssignableConcept<foo> >();
  }
  {
    typedef copy_constructible_archetype<> foo;
    function_requires< CopyConstructibleConcept<foo> >();
  }
  {
    typedef sgi_assignable_archetype<> foo;
    function_requires< SGIAssignableConcept<foo> >();
  }
  {
    typedef copy_constructible_archetype<> foo;
    typedef convertible_to_archetype<foo> convertible_to_foo;
    function_requires< ConvertibleConcept<convertible_to_foo, foo> >();
  }
  {
    function_requires< ConvertibleConcept<boolean_archetype, bool> >();
  }
  {
    typedef equality_comparable_archetype<> foo;
    function_requires< EqualityComparableConcept<foo> >();
  }
  {
    typedef less_than_comparable_archetype<> foo;
    function_requires< LessThanComparableConcept<foo> >();
  }
  {
    typedef comparable_archetype<> foo;
    function_requires< ComparableConcept<foo> >();
  }
  {
    typedef equal_op_first_archetype<> First;
    typedef equal_op_second_archetype<> Second;
    function_requires< EqualOpConcept<First, Second> >();
  }
  {
    typedef not_equal_op_first_archetype<> First;
    typedef not_equal_op_second_archetype<> Second;
    function_requires< NotEqualOpConcept<First, Second> >();
  }
  {
    typedef less_than_op_first_archetype<> First;
    typedef less_than_op_second_archetype<> Second;
    function_requires< LessThanOpConcept<First, Second> >();
  }
  {
    typedef less_equal_op_first_archetype<> First;
    typedef less_equal_op_second_archetype<> Second;
    function_requires< LessEqualOpConcept<First, Second> >();
  }
  {
    typedef greater_than_op_first_archetype<> First;
    typedef greater_than_op_second_archetype<> Second;
    function_requires< GreaterThanOpConcept<First, Second> >();
  }
  {
    typedef greater_equal_op_first_archetype<> First;
    typedef greater_equal_op_second_archetype<> Second;
    function_requires< GreaterEqualOpConcept<First, Second> >();
  }

  {
    typedef copy_constructible_archetype<> Return;
    typedef plus_op_first_archetype<Return> First;
    typedef plus_op_second_archetype<Return> Second;
    function_requires< PlusOpConcept<Return, First, Second> >();
  }

  //===========================================================================
  // Function Object Concepts

  {
    typedef generator_archetype<null_archetype<> > foo;
    function_requires< GeneratorConcept<foo, null_archetype<> > >();
  }
#if !defined BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
  {
    function_requires< GeneratorConcept< void_generator_archetype, void > >();
  }
#endif
  {
    typedef unary_function_archetype<int, int> F;
    function_requires< UnaryFunctionConcept<F, int, int> >();
  }
  {
    typedef binary_function_archetype<int, int, int> F;
    function_requires< BinaryFunctionConcept<F, int, int, int> >();
  }
  {
    typedef unary_predicate_archetype<int> F;
    function_requires< UnaryPredicateConcept<F, int> >();
  }
  {
    typedef binary_predicate_archetype<int, int> F;
    function_requires< BinaryPredicateConcept<F, int, int> >();
  }

  //===========================================================================
  // Iterator Concepts
  {
    typedef trivial_iterator_archetype<null_archetype<> > Iter;
    function_requires< TrivialIteratorConcept<Iter> >();
  }
  {
    typedef mutable_trivial_iterator_archetype<null_archetype<> > Iter;
    function_requires< Mutable_TrivialIteratorConcept<Iter> >();
  }
  {
    typedef input_iterator_archetype<null_archetype<> > Iter;
    function_requires< InputIteratorConcept<Iter> >();
  }
  {
    typedef output_iterator_archetype<int> Iter;
    function_requires< OutputIteratorConcept<Iter, int> >();
  }
  {
    typedef forward_iterator_archetype<null_archetype<> > Iter;
    function_requires< ForwardIteratorConcept<Iter> >();
  }
  {
    typedef mutable_forward_iterator_archetype<assignable_archetype<> > Iter;
    function_requires< Mutable_ForwardIteratorConcept<Iter> >();
  }
  {
    typedef bidirectional_iterator_archetype<null_archetype<> > Iter;
    function_requires< BidirectionalIteratorConcept<Iter> >();
  }
  {
    typedef mutable_bidirectional_iterator_archetype<assignable_archetype<> > 
      Iter;
    function_requires< Mutable_BidirectionalIteratorConcept<Iter> >();
  }
  {
    typedef random_access_iterator_archetype<null_archetype<> > Iter;
    function_requires< RandomAccessIteratorConcept<Iter> >();
  }
  {
    typedef mutable_random_access_iterator_archetype<assignable_archetype<> > 
      Iter;
    function_requires< Mutable_RandomAccessIteratorConcept<Iter> >();
  }

  //===========================================================================
  // Container Concepts

  // UNDER CONSTRUCTION

  return 0;
}
