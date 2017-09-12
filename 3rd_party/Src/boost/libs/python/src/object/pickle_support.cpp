//  (C) Copyright R.W. Grosse-Kunstleve 2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies. This
//  software is provided "as is" without express or implied warranty, and
//  with no claim as to its suitability for any purpose.

#include <boost/python/make_function.hpp>
#include <boost/python/object/class.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/list.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/detail/api_placeholder.hpp>

namespace boost { namespace python {

namespace {

  tuple instance_reduce(object instance_obj)
  {
      list result;
      object instance_class(instance_obj.attr("__class__"));
      result.append(instance_class);
      object none;
      object getinitargs = getattr(instance_obj, "__getinitargs__", none);
      tuple initargs;
      if (getinitargs.ptr() != none.ptr()) {
          initargs = tuple(getinitargs());
      }
      result.append(initargs);
      object getstate = getattr(instance_obj, "__getstate__", none);
      object instance_dict = getattr(instance_obj, "__dict__", none);
      long len_instance_dict = 0;
      if (instance_dict.ptr() != none.ptr()) {
          len_instance_dict = len(instance_dict);
      }
      if (getstate.ptr() != none.ptr()) {
          if (len_instance_dict > 0) {
              object getstate_manages_dict = getattr(
                instance_obj, "__getstate_manages_dict__", none);
              if (getstate_manages_dict.ptr() == none.ptr()) {
                  PyErr_SetString(PyExc_RuntimeError,
                    "Incomplete pickle support"
                    " (__getstate_manages_dict__ not set)");
                  throw_error_already_set();
              }
          }
          result.append(getstate());
      }
      else if (len_instance_dict > 0) {
          result.append(instance_dict);
      }
      return tuple(result);
  }

} // namespace

object const& make_instance_reduce_function()
{
    static object result(&instance_reduce);
    return result;
}

}} // namespace boost::python
