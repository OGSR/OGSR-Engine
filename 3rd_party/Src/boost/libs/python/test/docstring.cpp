// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/operators.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/manage_new_object.hpp>
#include <string>
#include "test_class.hpp"

// Just use math.h here; trying to use std::pow() causes too much
// trouble for non-conforming compilers and libraries.
#include <math.h>

using namespace boost::python;

typedef test_class<> X;

X* create(int x)
{
    return new X(x);
}

unsigned long fact(unsigned long n)
{
    return n <= 1 ? n : n * fact(n - 1);
}

BOOST_PYTHON_MODULE(docstring_ext)
{
    scope().attr("__doc__") =
        "A simple test module for documentation strings\n"
        "Exercised by docstring.py"
        ;

    class_<X>("X",
              "A simple class wrapper around a C++ int\n"
              "includes some error-checking"

              , init<int>(
                  "this is the __init__ function\n"
                  "its documentation has two lines."
                )

        )
        .def("value", &X::value,
             "gets the value of the object")
        ;

    def("create", create, return_value_policy<manage_new_object>(),
        "creates a new X object");

    def("fact", fact, "compute the factorial");
}

#include "module_tail.cpp"
