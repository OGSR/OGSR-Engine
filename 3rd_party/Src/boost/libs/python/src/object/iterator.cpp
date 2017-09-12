// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/object/iterator_core.hpp>
#include <boost/python/object/function_object.hpp>
#include <boost/bind.hpp>

namespace boost { namespace python { namespace objects { 

static PyObject* identity(PyObject* args_, PyObject*)
{
    PyObject* x = PyTuple_GET_ITEM(args_,0);
    Py_INCREF(x);
    return x;
}

BOOST_PYTHON_DECL object const& identity_function()
{
    static object result(function_object(&identity, 1));
    return result;
}

void set_stop_iteration_error()
{
    PyErr_SetObject(PyExc_StopIteration, Py_None);
}

}}} // namespace boost::python::objects
