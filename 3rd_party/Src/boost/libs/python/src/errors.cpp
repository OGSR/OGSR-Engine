// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#ifndef BOOST_PYTHON_SOURCE
# define BOOST_PYTHON_SOURCE
#endif

#include <boost/python/errors.hpp>
#include <boost/cast.hpp>
#include <boost/python/detail/exception_handler.hpp>

namespace boost { namespace python {

// IMPORTANT: this function may only be called from within a catch block!
BOOST_PYTHON_DECL bool handle_exception_impl(function0<void> f)
{
    try
    {
        if (detail::exception_handler::chain)
            return detail::exception_handler::chain->handle(f);
        f();
        return false;
    }
    catch(const boost::python::error_already_set&)
    {
        // The python error reporting has already been handled.
    }
    catch(const std::bad_alloc&)
    {
        PyErr_NoMemory();
    }
    catch(const bad_numeric_cast& x)
    {
        PyErr_SetString(PyExc_OverflowError, x.what());
    }
    catch(const std::exception& x)
    {
        PyErr_SetString(PyExc_RuntimeError, x.what());
    }
    catch(...)
    {
        PyErr_SetString(PyExc_RuntimeError, "unidentifiable C++ exception");
    }
    return true;
}

void BOOST_PYTHON_DECL throw_argument_error()
{
    throw argument_error();
}

void BOOST_PYTHON_DECL throw_error_already_set()
{
    throw error_already_set();
}

namespace detail {

  BOOST_PYTHON_DECL void expect_complex(PyObject* p)
  {
      if (!PyComplex_Check(p))
      {
          PyErr_SetString(PyExc_TypeError, "expected a complex number");
          boost::python::throw_argument_error();
      }
  }

// needed by void_adaptor (see void_adaptor.hpp)
BOOST_PYTHON_DECL PyObject arbitrary_object = { 0 };

bool exception_handler::operator()(function0<void> const& f) const
{
    if (m_next)
    {
        return m_next->handle(f);
    }
    else
    {
        f();
        return false;
    }
}

exception_handler::exception_handler(handler_function const& impl)
    : m_impl(impl)
    , m_next(0)
{
    if (chain != 0)
        tail->m_next = this;
    else
        chain = this;
    tail = this;
}

exception_handler* exception_handler::chain;
exception_handler* exception_handler::tail;

BOOST_PYTHON_DECL void register_exception_handler(handler_function const& f)
{
    // the constructor links the new object into a handler chain, so
    // this object isn't actaully leaked (until, of course, the
    // interpreter exits).
    new exception_handler(f);
}

} // namespace boost::python::detail

}} // namespace boost::python


