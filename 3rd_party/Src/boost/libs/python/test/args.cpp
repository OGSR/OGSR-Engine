// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/args.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/return_internal_reference.hpp>
#include "test_class.hpp"

using namespace boost::python;

tuple f(int x = 1, double y = 4.25, char const* z = "wow")
{
    return make_tuple(x, y, z);
}

BOOST_PYTHON_FUNCTION_OVERLOADS(f_overloads, f, 0, 3)
    
typedef test_class<> Y;

struct X
{
    X(int a0 = 0, int a1 = 1) : inner0(a0), inner1(a1) {}
    tuple f(int x = 1, double y = 4.25, char const* z = "wow")
    {
        return make_tuple(x, y, z);
    }

    Y const& inner(bool n) const { return n ? inner1 : inner0; }
    
    Y inner0;
    Y inner1;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_f_overloads, X::f, 0, 3)


tuple raw_func(tuple args, dict kw)
{
    return make_tuple(args, kw);
}

BOOST_PYTHON_MODULE(args_ext)
{
    def("f", f, args("x", "y", "z")
        , "This is f's docstring"
        );

    def("raw", raw_function(raw_func));
    
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1200
    // MSVC6 gives a fatal error LNK1179: invalid or corrupt file:
    // duplicate comdat error if we try to re-use the exact type of f
    // here, so substitute long for int.
    tuple (*f)(long,double,char const*) = 0;
#endif 
    def("f1", f, f_overloads("f1's docstring", args("x", "y", "z")));
    def("f2", f, f_overloads(args("x", "y", "z")));
    def("f3", f, f_overloads(args("x", "y", "z"), "f3's docstring"));
    
    class_<Y>("Y", init<int>(args("value"), "Y's docstring"))
        .def("value", &Y::value)
        .def("raw", raw_function(raw_func))
        ;
            
    class_<X>("X", "This is X's docstring")
        .def(init<int, optional<int> >(args("a0", "a1")))
        .def("f", &X::f
             , "This is X.f's docstring"
             , args("x", "y", "z"))

        // Just to prove that all the different argument combinations work
        .def("inner0", &X::inner, return_internal_reference<>(), args("n"), "docstring")
        .def("inner1", &X::inner, return_internal_reference<>(), "docstring", args("n"))

        .def("inner2", &X::inner, args("n"), return_internal_reference<>(), "docstring")
        .def("inner3", &X::inner, "docstring", return_internal_reference<>(), args("n"))

        .def("inner4", &X::inner, args("n"), "docstring", return_internal_reference<>())
        .def("inner5", &X::inner, "docstring", args("n"), return_internal_reference<>())

        .def("f1", &X::f, X_f_overloads(args("x", "y", "z")))
        ;

    def("inner", &X::inner, "docstring", args("self", "n"), return_internal_reference<>());
}
