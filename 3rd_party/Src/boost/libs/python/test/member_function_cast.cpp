// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/detail/member_function_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/type.hpp>
#include <boost/static_assert.hpp>

using namespace boost;

template <class T, class S>
void assert_same(S, type<T>* = 0)
{
    BOOST_STATIC_ASSERT((is_same<T,S>::value));
}

template <class Expected, class Target, class F>
void assert_mf_cast(F f, type<Expected>* = 0, type<Target>* = 0)
{
    assert_same<Expected>(
        python::detail::member_function_cast<Target,F>::stage1(f).stage2((Target*)0).stage3(f)
        );
}

struct X
{
    int f() const { return 0; }
    void g(char*) {}
};

struct Y : X
{
    
};

struct Z : Y
{
    int f() const { return 0; }
    void g(char*) {}
};

int main()
{
    assert_mf_cast<int (Y::*)() const, Y>(&X::f);
    assert_mf_cast<void (Y::*)(char*), Y>(&X::g);

    assert_mf_cast<int (Z::*)() const, Y>(&Z::f);
    assert_mf_cast<void (Z::*)(char*), Y>(&Z::g);

    assert_mf_cast<int, Y>(3);
    assert_mf_cast<X, Y>(X());
    return 0;
}
