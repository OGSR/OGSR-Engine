//  (C) Copyright David Abrahams 2000. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  The author gratefully acknowleges the support of Dragon Systems, Inc., in
//  producing this work.

#include <boost/python/scope.hpp>
#include <boost/python/object/add_to_namespace.hpp>

namespace boost { namespace python { namespace detail {

BOOST_PYTHON_DECL void scope_setattr_doc(char const* name, object const& x, char const* doc)
{
    // Use function::add_to_namespace to achieve overloading if
    // appropriate.
    scope current;
    objects::add_to_namespace(current, name, x, doc);
}

namespace
{
  PyMethodDef initial_methods[] = { { 0, 0, 0, 0 } };
}

BOOST_PYTHON_DECL void init_module(char const* name, void(*init_function)())
{
    
    PyObject* m
        = Py_InitModule(const_cast<char*>(name), initial_methods);

    if (m != 0)
    {
        // Create the current module scope
        scope current_module(
            (object(
                ((borrowed_reference_t*)m)
                ))
            );
        
        handle_exception(init_function);
    }
}

}}} // namespace boost::python::detail

namespace boost { namespace python {

BOOST_PYTHON_DECL PyObject* scope::current_scope = 0;

}}
