#include <boost/python/tuple.hpp>

namespace boost { namespace python { namespace detail {

detail::new_reference tuple_base::call(object const& arg_)
{
    return (detail::new_reference)PyObject_CallFunction(
        (PyObject*)&PyTuple_Type, "(O)", 
        arg_.ptr());
}
    
tuple_base::tuple_base()
    : object(detail::new_reference(PyTuple_New(0)))
{}
    
tuple_base::tuple_base(object_cref sequence)
    : object(call(sequence))
{}

}}}  // namespace boost::python
