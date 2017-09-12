// (C) Copyright David Abrahams 2001. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org for most recent version including documentation.
//
// Revision History
// 16 Feb 2001  Added a missing const. Made the tests run (somewhat) with
//              plain MSVC again. (David Abrahams)
// 11 Feb 2001  #if 0'd out use of counting_iterator on non-numeric types in
//              MSVC without STLport, so that the other tests may proceed
//              (David Abrahams)
// 04 Feb 2001  Added use of iterator_tests.hpp (David Abrahams)
// 28 Jan 2001  Removed not_an_iterator detritus (David Abrahams)
// 24 Jan 2001  Initial revision (David Abrahams)

#include <boost/config.hpp>
#ifdef BOOST_MSVC
# pragma warning(disable:4786) // identifier truncated in debug info
#endif

#include <boost/pending/iterator_tests.hpp>
#include <boost/counting_iterator.hpp>
#include <boost/detail/iterator.hpp>
#include <iostream>
#include <climits>
#include <iterator>
#include <stdlib.h>
#ifndef __BORLANDC__
# include <boost/tuple/tuple.hpp>
#endif 
#include <vector>
#include <list>
#include <cassert>
#ifndef BOOST_NO_LIMITS
# include <limits>
#endif
#ifndef BOOST_NO_SLIST
# include <slist>
#endif

template <class T> struct is_numeric
{
    enum { value = 
#ifndef BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
        std::numeric_limits<T>::is_specialized
#else
        // Causes warnings with GCC, but how else can I detect numeric types at
        // compile-time?
        (boost::is_convertible<int,T>::value &&
         boost::is_convertible<T,int>::value)
#endif
    };
};

// Special tests for RandomAccess CountingIterators.
template <class CountingIterator>
void category_test(
    CountingIterator start,
    CountingIterator finish,
    std::random_access_iterator_tag)
{
    typedef typename
        boost::detail::iterator_traits<CountingIterator>::difference_type
        difference_type;
    difference_type distance = boost::detail::distance(start, finish);

    // Pick a random position internal to the range
    difference_type offset = (unsigned)rand() % distance;
    assert(offset >= 0);
    CountingIterator internal = start;
    std::advance(internal, offset);

    // Try some binary searches on the range to show that it's ordered
    assert(std::binary_search(start, finish, *internal));

    // #including tuple crashed borland, so I had to give up on tie().
    std::pair<CountingIterator,CountingIterator> xy(
        std::equal_range(start, finish, *internal));
    CountingIterator x = xy.first, y = xy.second;
    
    assert(boost::detail::distance(x, y) == 1);

    // Show that values outside the range can't be found
    assert(!std::binary_search(start, boost::prior(finish), *finish));

    // Do the generic random_access_iterator_test
    typedef typename CountingIterator::value_type value_type;
    std::vector<value_type> v;
    for (value_type z = *start; z != *finish; ++z)
        v.push_back(z);
    if (v.size() >= 2)
    {
        // Note that this test requires a that the first argument is
        // dereferenceable /and/ a valid iterator prior to the first argument
        boost::random_access_iterator_test(start + 1, v.size() - 1, v.begin() + 1);
    }
}

// Special tests for bidirectional CountingIterators
template <class CountingIterator>
void category_test(CountingIterator start, CountingIterator finish, std::bidirectional_iterator_tag)
{
    if (finish != start
        && finish != boost::next(start)
        && finish != boost::next(boost::next(start)))
    {
        // Note that this test requires a that the first argument is
        // dereferenceable /and/ a valid iterator prior to the first argument
        boost::bidirectional_iterator_test(boost::next(start), boost::next(*start), boost::next(boost::next(*start)));
    }
}

template <class CountingIterator>
void category_test(CountingIterator start, CountingIterator finish, std::forward_iterator_tag)
{
    if (finish != start && finish != boost::next(start))
        boost::forward_iterator_test(start, *start, boost::next(*start));
}

template <class CountingIterator>
void test_aux(CountingIterator start, CountingIterator finish)
{
    typedef typename CountingIterator::iterator_category category;
    typedef typename CountingIterator::value_type value_type;

    // If it's a RandomAccessIterator we can do a few delicate tests
    category_test(start, finish, category());
    
    // Okay, brute force...
    for (CountingIterator p = start; p != finish && boost::next(p) != finish; ++p)
    {
        assert(boost::next(*p) == *boost::next(p));
    }

    // prove that a reference can be formed to these values
    typedef typename CountingIterator::value_type value;
    const value* q = &*start;
    (void)q; // suppress unused variable warning
}

template <class Incrementable>
void test(Incrementable start, Incrementable finish)
{
    test_aux(boost::make_counting_iterator(start), boost::make_counting_iterator(finish));
}

template <class Integer>
void test_integer(Integer* = 0) // default arg works around MSVC bug
{
    Integer start = 0;
    Integer finish = 120;
    test(start, finish);
}

template <class Container>
void test_container(Container* = 0)  // default arg works around MSVC bug
{
    Container c(1 + (unsigned)rand() % 1673);

    const typename Container::iterator start = c.begin();
    
    // back off by 1 to leave room for dereferenceable value at the end
    typename Container::iterator finish = start;
    std::advance(finish, c.size() - 1);
    
    test(start, finish);

    typedef typename Container::const_iterator const_iterator;
    test(const_iterator(start), const_iterator(finish));
}

class my_int1 {
public:
  my_int1() { }
  my_int1(int x) : m_int(x) { }
  my_int1& operator++() { ++m_int; return *this; }
  bool operator==(const my_int1& x) const { return m_int == x.m_int; }
private:
  int m_int;
};

namespace boost {
  template <>
  struct counting_iterator_traits<my_int1> {
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
  };
}

class my_int2 {
public:
  typedef void value_type;
  typedef void pointer;
  typedef void reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  my_int2() { }
  my_int2(int x) : m_int(x) { }
  my_int2& operator++() { ++m_int; return *this; }
  my_int2& operator--() { --m_int; return *this; }
  bool operator==(const my_int2& x) const { return m_int == x.m_int; }
private:
  int m_int;
};

class my_int3 {
public:
  typedef void value_type;
  typedef void pointer;
  typedef void reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;

  my_int3() { }
  my_int3(int x) : m_int(x) { }
  my_int3& operator++() { ++m_int; return *this; }
  my_int3& operator+=(std::ptrdiff_t n) { m_int += n; return *this; }
  std::ptrdiff_t operator-(const my_int3& x) const { return m_int - x.m_int; }
  my_int3& operator--() { --m_int; return *this; }
  bool operator==(const my_int3& x) const { return m_int == x.m_int; }
  bool operator!=(const my_int3& x) const { return m_int != x.m_int; }
  bool operator<(const my_int3& x) const { return m_int < x.m_int; }
private:
  int m_int;
};

int main()
{
    // Test the built-in integer types.
    test_integer<char>();
    test_integer<unsigned char>();
    test_integer<signed char>();
    test_integer<wchar_t>();
    test_integer<short>();
    test_integer<unsigned short>();
    test_integer<int>();
    test_integer<unsigned int>();
    test_integer<long>();
    test_integer<unsigned long>();
#if defined(BOOST_HAS_LONG_LONG)
    test_integer<long long>();
    test_integer<unsigned long long>();
#endif

   // wrapping an iterator or non-built-in integer type causes an INTERNAL
   // COMPILER ERROR in MSVC without STLport. I'm clueless as to why.
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1200 || defined(__SGI_STL_PORT)
    // Test user-defined type.
    test_integer<my_int1>();
    test_integer<my_int2>();
    test_integer<my_int3>();
    
   // Some tests on container iterators, to prove we handle a few different categories
    test_container<std::vector<int> >();
    test_container<std::list<int> >();
# ifndef BOOST_NO_SLIST
    test_container<BOOST_STD_EXTENSION_NAMESPACE::slist<int> >();
# endif

    // Also prove that we can handle raw pointers.
    int array[2000];
    test(boost::make_counting_iterator(array), boost::make_counting_iterator(array+2000-1));
#endif
    std::cout << "test successful " << std::endl;
    return 0;
}
