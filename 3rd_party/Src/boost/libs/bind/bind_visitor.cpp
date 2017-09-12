#if defined(_MSC_VER) && !defined(__ICL)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  bind_visitor.cpp - tests bind.hpp with a visitor
//
//  Copyright (c) 2001 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(push, 3)
#endif

#include <iostream>
#include <typeinfo>

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(pop)
#endif

// default implementation of visit_each

namespace boost
{
    template<class V, class T> void visit_each(V & v, T const & t, long)
    {
        v(t, 0);
    }
}

// visitor

int hash = 0;

struct visitor
{
    template<class T> void operator()(boost::reference_wrapper<T> const & r, int) const
    {
        std::cout << "Reference to " << typeid(T).name() << " @ " << &r.get() << " (with value " << r.get() << ")\n";
        hash += r.get();
    }

    template<class T> void operator()(T const &, long) const
    {
        std::cout << "Value of type " << typeid(T).name() << '\n';
        ++hash;
    }
};

int f(int & i, int & j, int)
{
    ++i;
    --j;
    return i + j;
}

int x = 2;
int y = 7;

int detect_errors(bool x)
{
    if(x)
    {
        std::cerr << "no errors detected.\n";
        return 0;
    }
    else
    {
        std::cerr << "test failed.\n";
        return 1;
    }
}

int main()
{
    using namespace boost;

    visitor v;
    visit_each(v, bind<int>(bind(f, ref(x), _1, 42), ref(y)), 0);

    return detect_errors(hash == 12);
}
