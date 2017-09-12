// (C) Copyright David Abrahams 2001. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org for most recent version including documentation.
//
// Revision History
// 11 Feb 2001  Compile with Borland, re-enable failing tests (David Abrahams)
// 29 Jan 2001  Initial revision (David Abrahams)

#include <boost/half_open_range.hpp>
#include <boost/utility.hpp>
#include <iterator>
#include <stdlib.h>
#include <vector>
#include <list>
#include <cassert>
#include <stdexcept>
#ifndef BOOST_NO_LIMITS
# include <limits>
#endif
#ifndef BOOST_NO_SLIST
# include <slist>
#endif

inline unsigned unsigned_random(unsigned max)
{
    return (max > 0) ? (unsigned)rand() % max : 0;
}

// Special tests for ranges supporting random access
template <class T>
void category_test_1(
    const boost::half_open_range<T>& r, std::random_access_iterator_tag)
{
    typedef boost::half_open_range<T> range;
    typedef typename range::size_type size_type;
    size_type size = r.size();

    // pick a random offset
    size_type offset = unsigned_random(size);

    typename range::value_type x = *(r.begin() + offset);
    // test contains(value_type)
    assert(r.contains(r.start()) == !r.empty());
    assert(!r.contains(r.finish()));
    assert(r.contains(x) == (offset != size));

    range::const_iterator p = r.find(x);
    assert((p == r.end()) == (x == r.finish()));
    assert(r.find(r.finish()) == r.end());

    if (offset != size)
    {
        assert(x == r[offset]);
        assert(x == r.at(offset));
    }

    bool caught_out_of_range = false;
    try {
        bool never_initialized = x == r.at(size);
        (void)never_initialized;
    }
    catch(std::out_of_range&)
    {
        caught_out_of_range = true;
    }
    catch(...)
    {
    }
    assert(caught_out_of_range);
}

// Those tests must be skipped for other ranges
template <class T>
void category_test_1(
    const boost::half_open_range<T>&, std::forward_iterator_tag)
{
}

unsigned indices[][2] = { {0,0},{0,1},{0,2},{0,3},
                                {1,1},{1,2},{1,3},
                                      {2,2},{2,3},
                                            {3,3}};

template <class Range>
void category_test_2(
    const std::vector<Range>& ranges, unsigned i, unsigned j, std::random_access_iterator_tag)
{
    typedef Range range;
    const range& ri = ranges[i];
    const range& rj = ranges[j];

    if (indices[i][0] <= indices[j][0] && indices[i][1] >= indices[j][1])
        assert(ri.contains(rj));

    if (ri.contains(rj))
        assert((ri & rj) == rj);
    assert(boost::intersects(ri, rj) == !(ri & rj).empty());

    range t1(ri);
    t1 &= rj;
    assert(t1 == range(indices[i][0] > indices[j][0] ? ri.start() : rj.start(),
                       indices[i][1] < indices[j][1] ? ri.finish() : rj.finish()));
    assert(t1 == (ri & rj));
    
    range t2(ri);
    t2 |= rj;
    
    if (ri.empty())
        assert(t2 == rj);
    else if (rj.empty())
        assert(t2 == ri);
    else
        assert(t2 == range(indices[i][0] < indices[j][0] ? ri.start() : rj.start(),
                           indices[i][1] > indices[j][1] ? ri.finish() : rj.finish()));
    assert(t2 == (ri | rj));
    if (i == j)
        assert(ri == rj);
    
    if (ri.empty() || rj.empty())
        assert((ri == rj) == (ri.empty() && rj.empty()));
    else
        assert((ri == rj) == (ri.start() == rj.start() && ri.finish() == rj.finish()));

    assert((ri == rj) == !(ri != rj));

    bool same = ri == rj;
    bool one_empty = ri.empty() != rj.empty();

    std::less<range> less;
    std::less_equal<range> less_equal;
    std::greater<range> greater;
    std::greater_equal<range> greater_equal;
    
    if (same)
    {
        assert(greater_equal(ri,rj));
        assert(less_equal(ri,rj));
        assert(!greater(ri,rj));
        assert(!less(ri,rj));
    }
    else if (one_empty)
    {
        const range& empty = ri.empty() ? ri : rj;
        const range& non_empty = rj.empty() ? ri : rj;
        
        assert(less(empty,non_empty));
        assert(less_equal(empty,non_empty));
        assert(!greater(empty,non_empty));
        assert(!greater_equal(empty,non_empty));
        assert(!less(non_empty,empty));
        assert(!less_equal(non_empty,empty));
        assert(greater(non_empty,empty));
        assert(greater_equal(non_empty,empty));
    }
    else {
        if (indices[i][0] < indices[j][0] ||
            indices[i][0] == indices[j][0] && indices[i][1] < indices[j][1])
        {
            assert(!greater_equal(ri,rj));
            assert(less(ri,rj));
        }

        if (indices[i][0] < indices[j][0] ||
            indices[i][0] == indices[j][0] && indices[i][1] <= indices[j][1])
        {
            assert(!greater(ri,rj));
            assert(less_equal(ri,rj));
        }

        if (indices[i][0] > indices[j][0] ||
            indices[i][0] == indices[j][0] && indices[i][1] > indices[j][1])
        {
            assert(!less_equal(ri,rj));
            assert(greater(ri,rj));
        }

        if (indices[i][0] > indices[j][0] ||
            indices[i][0] == indices[j][0] && indices[i][1] >= indices[j][1])
        {
            assert(!less(ri,rj));
            assert(greater_equal(ri,rj));
        }
    }
}


template <class Range>
void category_test_2(
    const std::vector<Range>&, unsigned, unsigned, std::forward_iterator_tag)
{
}

template <class T>
void category_test_2(
    const std::vector<boost::half_open_range<T> >&, unsigned, unsigned, std::bidirectional_iterator_tag)
{
}

template <class Range>
void test_back(Range& x, std::bidirectional_iterator_tag)
{
    assert(x.back() == boost::prior(x.finish()));
}

template <class Range>
void test_back(Range& x, std::forward_iterator_tag)
{
}

template <class T>
boost::half_open_range<T> range_identity(const boost::half_open_range<T>& x)
{
    return x;
}

template <class T>
void test(T x0, T x1, T x2, T x3)
{
    std::vector<boost::half_open_range<T> > ranges;
    typedef boost::half_open_range<T> range;

    T bounds[4] = { x0, x1, x2, x3 };

    const std::size_t num_ranges = sizeof(indices)/sizeof(*indices);
    // test construction
    for (std::size_t n = 0; n < num_ranges;++n)
    {
        T start = bounds[indices[n][0]];
        T finish = bounds[indices[n][1]];
        boost::half_open_range<T> r(start, finish);
        ranges.push_back(r);
    }
    
    // test implicit conversion from std::pair<T,T>
    range converted = std::pair<T,T>(x0,x0);
    (void)converted;

    // test assignment, equality and inequality
    range r00 = range(x0, x0);
    assert(r00 == range(x0,x0));
    assert(r00 == range(x1,x1)); // empty ranges are all equal
    if (x3 != x0)
        assert(r00 != range(x0, x3));
    r00 = range(x0, x3);
    assert(r00 == range(x0, x3));
    if (x3 != x0)
        assert(r00 != range(x0, x0));

    typedef typename range::iterator iterator;
    typedef typename iterator::iterator_category category;
    
    for (unsigned i = 0; i < num_ranges; ++i)
    {
        const range& r = ranges[i];
            
        // test begin(), end(), basic iteration.
        unsigned count = 0;
        for (range::const_iterator p = r.begin(), finish = r.end();
             p != finish;
             ++p, ++count)
        {
            assert(count < 2100);
        }

        // test size(), empty(), front(), back()
        assert((unsigned)r.size() == count);
        if (indices[i][0] == indices[i][1])
            assert(r.empty());
        if (r.empty())
            assert(r.size() == 0);
        if (!r.empty())
        {
            assert(r.front() == r.start());
            test_back(r, category());
        }

            // test swap
        range r1(r);
        range r2(x0,x3);
        const bool same = r1 == r2;
        r1.swap(r2);
        assert(r1 == range(x0,x3));
        assert(r2 == r);
        if (!same) {
            assert(r1 != r);
            assert(r2 != range(x0,x3));
        }

        // do individual tests for random-access iterators
        category_test_1(r, category());
    }

    for (unsigned j = 0; j < num_ranges; ++j) {
        for (unsigned k = 0; k < num_ranges; ++k) {
            category_test_2(ranges, j, k, category());
        }
    }
    
}

template <class Integer>
void test_integer(Integer* = 0) // default arg works around MSVC bug
{
    Integer a = 0;
    Integer b = a + unsigned_random(128 - a);
    Integer c = b + unsigned_random(128 - b);
    Integer d = c + unsigned_random(128 - c);

    test(a, b, c, d);
}

template <class Container>
void test_container(Container* = 0)  // default arg works around MSVC bug
{
    Container c(unsigned_random(1673));

    const typename Container::size_type offset1 = unsigned_random(c.size());
    const typename Container::size_type offset2 = unsigned_random(c.size() - offset1);
    typename Container::iterator internal1 = c.begin();
    std::advance(internal1, offset1);
    typename Container::iterator internal2 = internal1;
    std::advance(internal2, offset2);
    
    test(c.begin(), internal1, internal2, c.end());

    typedef typename Container::const_iterator const_iterator;
    test(const_iterator(c.begin()),
         const_iterator(internal1),
         const_iterator(internal2),
         const_iterator(c.end()));
}

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
    // Some tests on container iterators, to prove we handle a few different categories
    test_container<std::vector<int> >();
    test_container<std::list<int> >();
#ifndef BOOST_NO_SLIST
    test_container<BOOST_STD_EXTENSION_NAMESPACE::slist<int> >();
#endif
    // Also prove that we can handle raw pointers.
    int array[2000];
    const std::size_t a = 0;
    const std::size_t b = a + unsigned_random(2000 - a);
    const std::size_t c = b + unsigned_random(2000 - b);
    test(array, array+b, array+c, array+2000);
    return 0;
}
