// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/borrowed.hpp>
#include <boost/static_assert.hpp>

using namespace boost::python;

template <class T>
void assert_borrowed_ptr(T const& x)
{
    BOOST_STATIC_ASSERT(boost::python::detail::is_borrowed_ptr<T>::value);
}
    
template <class T>
void assert_not_borrowed_ptr(T const& x)
{
    BOOST_STATIC_ASSERT(!boost::python::detail::is_borrowed_ptr<T>::value);
}
    
int main()
{
    assert_borrowed_ptr(borrowed((PyObject*)0));
    assert_borrowed_ptr(borrowed((PyTypeObject*)0));
    assert_borrowed_ptr((detail::borrowed<PyObject> const*)0);
    assert_borrowed_ptr((detail::borrowed<PyObject> volatile*)0);
    assert_borrowed_ptr((detail::borrowed<PyObject> const volatile*)0);
    assert_not_borrowed_ptr((PyObject*)0);
    assert_not_borrowed_ptr(0);
    return 0;
}
