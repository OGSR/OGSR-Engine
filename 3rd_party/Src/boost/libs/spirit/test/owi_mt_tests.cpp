/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// vim:ts=4:sw=4:et

#include <iostream>
#if !defined(DONT_HAVE_BOOST)
#include <boost/thread/thread.hpp>
#endif
#if !defined(BOOST_HAS_THREADS)
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
    std::cout << "          object_with_id test (MT)\n";
    std::cout << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";

    std::cout << "Test skipped\n";
    return 0;
}
#else

#undef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE

#include <boost/spirit/core/non_terminal/impl/object_with_id.ipp>
#include <boost/ref.hpp>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdlib>

using boost::spirit::impl::object_with_id;

struct tag1 {};

typedef object_with_id<tag1> class1;

struct test_task
{
    static unsigned int size() { return 200000; }

    void operator ()()
    { // create lots of objects
        v1.reserve(size());
        for (unsigned long i=0; i<size(); ++i)
        {
            //boost::thread().yield();
            v1.push_back(new class1);
        }
    }

    std::vector<class1*> v1;
};

void
check_ascending(test_task const &t)
{
    typedef std::vector<class1*>::const_iterator iter;
    iter p(t.v1.begin());
    iter const e(t.v1.end());
    iter n(p);

    while (++n!=e)
    {
        if ((**n).get_object_id()<=(**p).get_object_id())
        {
            using namespace std;
            cerr << "object ids out of order";
            exit(EXIT_FAILURE);
        }
        p = n;
    }
};

struct less1
{
    bool operator()(class1 const *p, class1 const *q) const
    {
        return p->get_object_id() < q->get_object_id();
    }
};

void
check_not_contained_in(
    test_task const &candidate,
    test_task const &in
)
{
    typedef std::vector<class1*>::const_iterator iter;
    iter p(candidate.v1.begin());
    iter const e(candidate.v1.end());

    while (p!=e)
    {
        iter found = std::lower_bound(in.v1.begin(),in.v1.end(),*p,less1());
        if  (found!=in.v1.end() &&
            (**found).get_object_id() == (**p).get_object_id())
        {
            using namespace std;
            cerr << "object ids not unique";
            exit(EXIT_FAILURE);
        }
        ++p;
    }
};

int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
    std::cout << "          object_with_id test (MT)\n";
    std::cout << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";

    test_task test1;
    test_task test2;
    test_task test3;

    boost::thread thread1(boost::ref(test1));
    boost::thread thread2(boost::ref(test2));
    boost::thread thread3(boost::ref(test3));

    std::cout << "preparing ..." << std::flush;
    thread1.join();
    thread2.join();
    thread3.join();

    // now all objects should have unique ids,
    // the ids must be ascending within each vector
    std::cout << "checking \n";

    assert(test1.v1.size()==test_task::size());
    assert(test2.v1.size()==test_task::size());
    assert(test3.v1.size()==test_task::size());

    // check for ascending ids
    check_ascending(test1);
    check_ascending(test2);
    check_ascending(test3);

    //  check for uniqueness
    check_not_contained_in(test1,test3);
    check_not_contained_in(test1,test2);
    check_not_contained_in(test2,test1);
    check_not_contained_in(test2,test3);
    check_not_contained_in(test3,test2);
    check_not_contained_in(test3,test1);

    std::cout << "Test concluded successfully\n";
}

#endif // !defined(DONT_HAVE_BOOST)
