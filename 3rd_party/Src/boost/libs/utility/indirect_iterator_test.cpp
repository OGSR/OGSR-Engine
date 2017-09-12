//  (C) Copyright Jeremy Siek 1999. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

//  Revision History
//  08 Mar 2001   Jeremy Siek
//       Moved test of indirect iterator into its own file. It to
//       to be in iterator_adaptor_test.cpp.

#include <boost/config.hpp>
#include <iostream>
#include <algorithm>

#include <boost/iterator_adaptors.hpp>
#include <boost/pending/iterator_tests.hpp>
#include <boost/concept_archetype.hpp>
#include <stdlib.h>
#include <deque>
#include <set>

struct my_iterator_tag : public std::random_access_iterator_tag { };

using boost::dummyT;

typedef std::deque<int> storage;
typedef std::deque<int*> pointer_deque;
typedef std::set<storage::iterator> iterator_set;

void more_indirect_iterator_tests()
{
// For some reason all heck breaks loose in the compiler under these conditions.
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1200 || !defined(__STL_DEBUG)
    storage store(1000);
    std::generate(store.begin(), store.end(), rand);
    
    pointer_deque ptr_deque;
    iterator_set iter_set;

    for (storage::iterator p = store.begin(); p != store.end(); ++p)
    {
        ptr_deque.push_back(&*p);
        iter_set.insert(p);
    }

    typedef boost::indirect_iterator_pair_generator<
        pointer_deque::iterator
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
        , int
#endif
    > IndirectDeque;

    IndirectDeque::iterator db(ptr_deque.begin());
    IndirectDeque::iterator de(ptr_deque.end());
    assert(static_cast<std::size_t>(de - db) == store.size());
    assert(db + store.size() == de);
    IndirectDeque::const_iterator dci(db);
    assert(db == dci);
    assert(dci == db);
    assert(dci != de);
    assert(dci < de);
    assert(dci <= de);
    assert(de >= dci);
    assert(de > dci);
    dci = de;
    assert(dci == de);

    boost::random_access_iterator_test(db + 1, store.size() - 1, boost::next(store.begin()));
    
    *db = 999;
    assert(store.front() == 999);

    // Borland C++ is getting very confused about the typedef's here

    typedef boost::indirect_iterator_generator<
        iterator_set::iterator
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
        , int
#endif
        >::type indirect_set_iterator;

    typedef boost::indirect_iterator_generator<
        iterator_set::iterator,
        const int
        >::type const_indirect_set_iterator;

    indirect_set_iterator sb(iter_set.begin());
    indirect_set_iterator se(iter_set.end());
    const_indirect_set_iterator sci(iter_set.begin());
    assert(sci == sb);
    assert(sci != se);
    sci = se;
    assert(sci == se);
    
    *boost::prior(se) = 888;
    assert(store.back() == 888);
    assert(std::equal(sb, se, store.begin()));

    boost::bidirectional_iterator_test(boost::next(sb), store[1], store[2]);
    assert(std::equal(db, de, store.begin()));

#endif    
}

int
main()
{
  dummyT array[] = { dummyT(0), dummyT(1), dummyT(2), 
                     dummyT(3), dummyT(4), dummyT(5) };
  const int N = sizeof(array)/sizeof(dummyT);

  // Test indirect_iterator_generator
  {
    dummyT* ptr[N];
    for (int k = 0; k < N; ++k)
      ptr[k] = array + k;

    typedef boost::indirect_iterator_generator<dummyT**
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
        , dummyT
#endif
      >::type indirect_iterator;

    typedef boost::indirect_iterator_generator<dummyT**, const dummyT>::type const_indirect_iterator;

    indirect_iterator i(ptr);
    boost::random_access_iterator_test(i, N, array);

#ifndef BOOST_NO_STD_ITERATOR_TRAITS
    boost::random_access_iterator_test(boost::make_indirect_iterator(ptr), N, array);
#endif
    
    // check operator->
    assert((*i).m_x == i->foo());

    const_indirect_iterator j(ptr);
    boost::random_access_iterator_test(j, N, array);

    dummyT*const* const_ptr = ptr;
    
#ifndef BOOST_NO_STD_ITERATOR_TRAITS
    boost::random_access_iterator_test(boost::make_indirect_iterator(const_ptr), N, array);
#endif
    boost::const_nonconst_iterator_test(i, ++j);

    more_indirect_iterator_tests();
  }
  std::cout << "test successful " << std::endl;
  return 0;
}
