// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/converter/to_python_function_type.hpp>

struct hopefully_illegal
{
    static PyObject* convert(int&);
};

PyObject* x = boost::python::converter::as_to_python_function<int, hopefully_illegal>::convert(0);
