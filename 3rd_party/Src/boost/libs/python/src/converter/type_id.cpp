// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/type_id.hpp>
#include <boost/python/detail/decorated_type_id.hpp>
#if !defined(__GNUC__) || __GNUC__ >= 3 || __SGI_STL_PORT
# include <ostream>
#else 
# include <ostream.h>
#endif 

namespace boost { namespace python {

BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, type_info const& x)
{
    return os << x.name();
}

namespace detail
{
  BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, detail::decorated_type_info const& x)
  {
      os << x.m_base_type;
      if (x.m_decoration & decorated_type_info::const_)
          os << " const";
      if (x.m_decoration & decorated_type_info::volatile_)
          os << " volatile";
      if (x.m_decoration & decorated_type_info::reference)
          os << "&";
      return os;
  }
}
}} // namespace boost::python::converter
