// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/long.hpp>

namespace boost { namespace python { namespace detail {

new_non_null_reference long_base::call(object const& arg_)
{
    return (detail::new_non_null_reference)PyObject_CallFunction(
        (PyObject*)&PyLong_Type, "(O)", 
        arg_.ptr());
}

new_non_null_reference long_base::call(object const& arg_, object const& base)
{
    return (detail::new_non_null_reference)PyObject_CallFunction(
        (PyObject*)&PyLong_Type, "(OO)", 
        arg_.ptr(), base.ptr());
}

long_base::long_base()
    : object(
        detail::new_reference(
            PyObject_CallFunction((PyObject*)&PyLong_Type, "()"))
        )
{}

long_base::long_base(object_cref arg)
    : object(long_base::call(arg))
{}

long_base::long_base(object_cref arg, object_cref base)
    : object(long_base::call(arg, base))
{}


}}} // namespace boost::python
