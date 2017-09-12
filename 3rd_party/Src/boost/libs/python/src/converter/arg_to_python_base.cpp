// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/converter/arg_to_python_base.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/converter/registrations.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/refcount.hpp>

namespace boost { namespace python { namespace converter { 

namespace detail
{
  arg_to_python_base::arg_to_python_base(
      void const volatile* source, registration const& converters)
# if !defined(BOOST_MSVC) || BOOST_MSVC <= 1300 || _MSC_FULL_VER > 13102179
      : handle<>
# else
      : m_ptr
# endif
         (converters.to_python(source))
  {
  }
}

}}} // namespace boost::python::converter
