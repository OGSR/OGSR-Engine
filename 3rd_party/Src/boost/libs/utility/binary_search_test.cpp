// (C) Copyright David Abrahams 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <vector>
#include <string>
#include <memory>
#include <climits>
#include <iostream>
#include <cassert>
#include <stdlib.h> // for rand(). Would use cstdlib but VC6.4 doesn't put it in std::
#include <list>
#include <algorithm>
#include <boost/detail/binary_search.hpp>

#if defined(__SGI_STL_PORT) ? defined(__SGI_STL_OWN_IOSTREAMS) : (!defined(__GNUC__) || __GNUC__ > 2)
# define USE_SSTREAM
#endif

#ifdef USE_SSTREAM
# include <sstream>
#else
# include <strstream>
#endif

namespace {

typedef std::vector<std::string> string_vector;

const std::size_t sequence_length = 1000;

unsigned random_number()
{
    return static_cast<unsigned>(::rand()) % sequence_length;
}

# ifndef USE_SSTREAM
class unfreezer {
 public:
    unfreezer(std::ostrstream& s) : m_stream(s) {}
    ~unfreezer() { m_stream.freeze(false); }
 private:
    std::ostrstream& m_stream;
};
# endif

template <class T>
void push_back_random_number_string(T& seq)
{
    unsigned value = random_number();
# if defined(__SGI_STL_PORT) ? defined(__SGI_STL_OWN_IOSTREAMS) : (!defined(__GNUC__) || __GNUC__ > 2)
    std::ostringstream s;
    s << value;
    seq.push_back(s.str());
# else
    std::ostrstream s;
    auto unfreezer unfreeze(s);
    s << value << char(0);
    seq.push_back(std::string(s.str()));
# endif
}

inline unsigned to_int(unsigned x) { return x; }
inline unsigned to_int(const std::string& x) { return atoi(x.c_str()); }

struct cmp
{
    template <class A1, class A2>
    inline bool operator()(const A1& a1, const A2& a2) const
    {
        return to_int(a1) < to_int(a2);
    }
};

inline bool operator<(const std::string& x, const unsigned y)
{
    return to_int(x) < y;
}

inline bool operator<(const unsigned y, const std::string& x)
{
    return y < to_int(x);
}

template <class T> void sort_by_value(T&);

template <>
void sort_by_value(std::vector<std::string>& v)
{
    std::sort(v.begin(), v.end(), cmp());
}

template <class T>
void random_sorted_sequence(T& seq)
{
    seq.clear();
    for (std::size_t i = 0; i < sequence_length; ++i)
    {
        push_back_random_number_string(seq);
    }
    sort_by_value(seq);
}

# if defined(BOOST_MSVC) && BOOST_MSVC < 1300 && !defined(__SGI_STL_PORT)
// VC6's standard lib doesn't have a template member function for list::sort()
template <>
void random_sorted_sequence(std::list<std::string>& result)
{
    std::vector<std::string> seq;
    seq.reserve(sequence_length);
    for (std::size_t i = 0; i < sequence_length; ++i)
    {
        push_back_random_number_string(seq);
    }
    sort_by_value(seq);
    result.resize(seq.size());
    std::copy(seq.begin(), seq.end(), result.begin());
}
#else
template <>
void sort_by_value(std::list<std::string>& l)
{
    l.sort(cmp());
}
# endif

// A way to select the comparisons with/without a Compare parameter for testing.
template <class Compare> struct searches
{
    template <class Iterator, class Key>
    static Iterator lower_bound(Iterator start, Iterator finish, Key key, Compare cmp)
        { return boost::detail::lower_bound(start, finish, key, cmp); }

    template <class Iterator, class Key>
    static Iterator upper_bound(Iterator start, Iterator finish, Key key, Compare cmp)
        { return boost::detail::upper_bound(start, finish, key, cmp); }

    template <class Iterator, class Key>
    static std::pair<Iterator, Iterator> equal_range(Iterator start, Iterator finish, Key key, Compare cmp)
        { return boost::detail::equal_range(start, finish, key, cmp); }

    template <class Iterator, class Key>
    static bool binary_search(Iterator start, Iterator finish, Key key, Compare cmp)
        { return boost::detail::binary_search(start, finish, key, cmp); }
};

struct no_compare {};

template <> struct searches<no_compare>
{
    template <class Iterator, class Key>
    static Iterator lower_bound(Iterator start, Iterator finish, Key key, no_compare)
        { return boost::detail::lower_bound(start, finish, key); }

    template <class Iterator, class Key>
    static Iterator upper_bound(Iterator start, Iterator finish, Key key, no_compare)
        { return boost::detail::upper_bound(start, finish, key); }

    template <class Iterator, class Key>
    static std::pair<Iterator, Iterator> equal_range(Iterator start, Iterator finish, Key key, no_compare)
        { return boost::detail::equal_range(start, finish, key); }

    template <class Iterator, class Key>
    static bool binary_search(Iterator start, Iterator finish, Key key, no_compare)
        { return boost::detail::binary_search(start, finish, key); }
};

template <class Sequence, class Compare>
void test_loop(Sequence& x, Compare cmp, unsigned long test_count)
{
    typedef typename Sequence::const_iterator const_iterator;
    
    for (unsigned long i = 0; i < test_count; ++i)
    {
        random_sorted_sequence(x);
        const const_iterator start = x.begin();
        const const_iterator finish = x.end();
        
        unsigned key = random_number();
        const const_iterator l = searches<Compare>::lower_bound(start, finish, key, cmp);
        const const_iterator u = searches<Compare>::upper_bound(start, finish, key, cmp);

        bool found_l = false;
        bool found_u = false;
        std::size_t index = 0;
        std::size_t count = 0;
        unsigned last_value = 0;
        for (const_iterator p = start; p != finish; ++p)
        {
            if (p == l)
                found_l = true;
            
            if (p == u)
            {
                assert(found_l);
                found_u = true;
            }

            unsigned value = to_int(*p);
            assert(value >= last_value);
            last_value = value;
            
            if (!found_l)
            {
                ++index;
                assert(to_int(*p) < key);
            }
            else if (!found_u)
            {
                ++count;
                assert(to_int(*p) == key);
            }
            else
                assert(to_int(*p) > key);
        }
        assert(found_l || l == finish);
        assert(found_u || u == finish);

        std::pair<const_iterator, const_iterator>
            range = searches<Compare>::equal_range(start, finish, key, cmp);
        assert(range.first == l);
        assert(range.second == u);

        bool found = searches<Compare>::binary_search(start, finish, key, cmp);
        assert(found == (u != l));
        std::cout << "found " << count << " copies of " << key << " at index " << index << "\n";
    }
}

}

int main()
{
    std::vector<std::string> x;
    std::cout << "=== testing random-access iterators with <: ===\n";
    test_loop(x, no_compare(), 25);
    std::cout << "=== testing random-access iterators with compare: ===\n";
    test_loop(x, cmp(), 25);
    
    std::list<std::string> y;
    std::cout << "=== testing bidirectional iterators with <: ===\n";
    test_loop(y, no_compare(), 25);
    std::cout << "=== testing bidirectional iterators with compare: ===\n";
    test_loop(y, cmp(), 25);
    std::cerr << "******TEST PASSED******\n";
    return 0;
}
