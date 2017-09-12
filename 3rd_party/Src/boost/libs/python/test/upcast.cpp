// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/cast.hpp>

struct X { long x; };
struct Y : X, PyObject {};

int main()
{
    PyTypeObject o;
    Y y;
    assert(&boost::python::upcast<PyObject>(&o)->ob_refcnt == &o.ob_refcnt);
    assert(&boost::python::upcast<PyObject>(&y)->ob_refcnt == &y.ob_refcnt);
    return 0;
}

