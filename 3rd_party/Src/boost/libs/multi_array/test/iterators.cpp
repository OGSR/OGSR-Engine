//
// iterators.cpp - checking out iterator stuffs. 
//    The tests assume that the array has shape 2x3x4
//

#define MULTIARRAY_TEST_ASSIGN
#include "generative_tests.hpp"
#include "boost/concept_check.hpp" // for ignore_unused_variable_warning

// iterator-test-specific code

template <typename Array>
void assign_if_not_const(Array& A, const mutable_array_tag&) {

  typedef typename Array::iterator iterator3;
  typedef typename Array::template subarray<2>::type::iterator iterator2;
  typedef typename Array::template subarray<1>::type::iterator iterator1;

  int num = 0;
  for (iterator3 i = A.begin(); i != A.end(); ++i)
    for(iterator2 ii = (*i).begin(); ii != (*i).end(); ++ii)
      for(iterator1  iii = (*ii).begin(); iii != (*ii).end(); ++iii)
        *iii = num++;
}


template <typename Array>
struct ittraits_const {
  typedef typename Array::const_iterator iterator3;
  typedef typename boost::subarray_gen<Array,2>::type::const_iterator
  iterator2;
  typedef typename boost::subarray_gen<Array,1>::type::const_iterator
  iterator1;

  typedef typename Array::const_reverse_iterator riterator3;
  typedef typename boost::subarray_gen<Array,2>::type::const_reverse_iterator
  riterator2;
  typedef typename boost::subarray_gen<Array,1>::type::const_reverse_iterator
  riterator1;
};

template <typename Array>
struct ittraits_mutable {
  typedef typename Array::iterator iterator3;
  typedef typename boost::subarray_gen<Array,2>::type::iterator iterator2;
  typedef typename boost::subarray_gen<Array,1>::type::iterator iterator1;

  typedef typename Array::reverse_iterator riterator3;
  typedef typename boost::subarray_gen<Array,2>::type::reverse_iterator
  riterator2;
  typedef typename boost::subarray_gen<Array,1>::type::reverse_iterator
  riterator1;
};

/////////////////////////////////////////////////////////////////////////
// choose ittraits begins
//

struct choose_ittraits_const {
  template <typename Array>
  struct bind {
    typedef ittraits_const<Array> type;
  };
};

struct choose_ittraits_mutable {
  template <typename Array>
  struct bind {
    typedef ittraits_mutable<Array> type;
  };
};


template <typename ConstnessTag>
struct ittraits_gen_helper {
  typedef choose_ittraits_mutable choice;
};

template <>
struct ittraits_gen_helper<const_array_tag> {
  typedef choose_ittraits_const choice;
};

template <typename Array, typename ConstTag>
struct ittraits_generator {
private:
  typedef typename ittraits_gen_helper<ConstTag>::choice Choice;
public:
  typedef typename Choice::template bind<Array>::type type;
};

//
// choose ittraits ends
/////////////////////////////////////////////////////////////////////////


template <typename Array>
void construct_iterators(Array&) {

  // Default constructed iterators and
  // const iterators constructed from iterators.
  {
    typename Array::iterator i1;
    typename Array::const_iterator ci1;
    typename Array::reverse_iterator r1;
    typename Array::const_reverse_iterator cr1;

#if 0 // RG - MSVC fails to compile these
    typename Array::const_iterator ci2 = 
      typename Array::iterator();
    typename Array::const_reverse_iterator cr2 = 
      typename Array::reverse_iterator();
#endif
    typename Array::const_iterator ci2 = i1;
    typename Array::const_reverse_iterator cr2 = cr1;
    boost::ignore_unused_variable_warning(cr2);
  }
}

template <typename Array, typename IterTraits>
void test_iterators(Array& A, const IterTraits&) {

  // Iterator comparison and arithmetic
  {
    typedef typename IterTraits::iterator3 iterator;
    iterator i1 = A.begin();
    iterator i2 = A.end();
    BOOST_TEST(i1 < i2);
    BOOST_TEST((i2 - i1) == typename iterator::difference_type(2));
  }

  // Standard Array Iteration
  {
    typedef typename IterTraits::iterator3 iterator3;
    typedef typename IterTraits::iterator2 iterator2;
    typedef typename IterTraits::iterator1 iterator1;
    
    int vals = 0;
    for (iterator3 i = A.begin(); i != A.end(); ++i)
      for(iterator2 ii = (*i).begin(); ii != (*i).end(); ++ii)
        for(iterator1  iii = (*ii).begin(); iii != (*ii).end(); ++iii)
          BOOST_TEST(*iii == vals++);
  }

  // Using operator->() on iterators
  {
    typedef typename IterTraits::iterator3 iterator3;
    typedef typename IterTraits::iterator2 iterator2;
    typedef typename IterTraits::iterator1 iterator1;
    
    int vals = 0;
    for (iterator3 i = A.begin(); i != A.end(); ++i)
      for(iterator2 ii = i->begin(); ii != i->end(); ++ii)
        for(iterator1  iii = ii->begin(); iii != ii->end(); ++iii)
          BOOST_TEST(*iii == vals++);
  }

  // Reverse Iterator Hierarchy Test
  {
    typedef typename IterTraits::riterator3 riterator3;
    typedef typename IterTraits::riterator2 riterator2;
    typedef typename IterTraits::riterator1 riterator1;

    int check_iter_val = A.num_elements()-1;
    for (riterator3 i = A.rbegin(); i != (riterator3)A.rend(); ++i)
      for(riterator2 ii = (*i).rbegin(); ii != (riterator2)(*i).rend(); ++ii)
        for(riterator1 iii = (*ii).rbegin(); iii != (riterator1)(*ii).rend();
            ++iii)
          BOOST_TEST(*iii == check_iter_val--);
  }
  ++tests_run;
}


template <typename Array>
void access(Array& A, const mutable_array_tag&) {
  assign(A);

  construct_iterators(A);
  
  typedef typename ittraits_generator<Array,mutable_array_tag>::type
    m_iter_traits;

  typedef typename ittraits_generator<Array,const_array_tag>::type
    c_iter_traits;
  test_iterators(A,m_iter_traits());
  test_iterators(A,c_iter_traits());

  const Array& CA = A;
  test_iterators(CA,c_iter_traits());
}

template <typename Array>
void access(Array& A, const const_array_tag&) {
  construct_iterators(A);
  typedef typename ittraits_generator<Array,const_array_tag>::type
    c_iter_traits;
  test_iterators(A,c_iter_traits());
}


int
test_main(int, char*[])
{
  return run_generative_tests();
}
