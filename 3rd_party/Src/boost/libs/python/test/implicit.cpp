// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/class.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include "test_class.hpp"

using namespace boost::python;

typedef test_class<> X;

int x_value(X const& x)
{
    return x.value();
}

X make_x(int n) { return X(n); }

BOOST_PYTHON_MODULE(implicit_ext)
{
    implicitly_convertible<int,X>();
    
    def("x_value", x_value);
    def("make_x", make_x);

    class_<X>("X", init<int>())
        .def("value", &X::value)
        .def("set", &X::set)
        ;
    
    implicitly_convertible<X,int>();
}

#include "module_tail.cpp"
