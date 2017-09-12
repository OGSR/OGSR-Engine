// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/object_operators.hpp>
#include <boost/python/detail/raw_pyobject.hpp>

namespace boost { namespace python { namespace api {

#define BOOST_PYTHON_BINARY_OPERATOR(op, name)                          \
BOOST_PYTHON_DECL object operator op(object const& l, object const& r)  \
{                                                                       \
    return object(                                                      \
        detail::new_reference(                                          \
            PyNumber_##name(l.ptr(), r.ptr()))                          \
        );                                                              \
}

BOOST_PYTHON_BINARY_OPERATOR(+, Add)
BOOST_PYTHON_BINARY_OPERATOR(-, Subtract)
BOOST_PYTHON_BINARY_OPERATOR(*, Multiply)
BOOST_PYTHON_BINARY_OPERATOR(/, Divide)
BOOST_PYTHON_BINARY_OPERATOR(%, Remainder)
BOOST_PYTHON_BINARY_OPERATOR(<<, Lshift)
BOOST_PYTHON_BINARY_OPERATOR(>>, Rshift)
BOOST_PYTHON_BINARY_OPERATOR(&, And)
BOOST_PYTHON_BINARY_OPERATOR(^, Xor)
BOOST_PYTHON_BINARY_OPERATOR(|, Or)
#undef BOOST_PYTHON_BINARY_OPERATOR

#define BOOST_PYTHON_INPLACE_OPERATOR(op, name)                         \
BOOST_PYTHON_DECL object& operator op##=(object& l, object const& r)    \
{                                                                       \
    return l = object(                                                  \
        (detail::new_reference)                                         \
            PyNumber_InPlace##name(l.ptr(), r.ptr()));                  \
}
    
BOOST_PYTHON_INPLACE_OPERATOR(+, Add)
BOOST_PYTHON_INPLACE_OPERATOR(-, Subtract)
BOOST_PYTHON_INPLACE_OPERATOR(*, Multiply)
BOOST_PYTHON_INPLACE_OPERATOR(/, Divide)
BOOST_PYTHON_INPLACE_OPERATOR(%, Remainder)
BOOST_PYTHON_INPLACE_OPERATOR(<<, Lshift)
BOOST_PYTHON_INPLACE_OPERATOR(>>, Rshift)
BOOST_PYTHON_INPLACE_OPERATOR(&, And)
BOOST_PYTHON_INPLACE_OPERATOR(^, Xor)
BOOST_PYTHON_INPLACE_OPERATOR(|, Or)
#undef BOOST_PYTHON_INPLACE_OPERATOR

object::object(handle<> const& x)
     : object_base(python::incref(python::expect_non_null(x.get())))
{}

}}} // namespace boost::python
