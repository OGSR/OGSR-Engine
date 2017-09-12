#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>

struct error {};

void translate(error const& e)
{
    PyErr_SetString(PyExc_RuntimeError, "!!!error!!!");
}

void throw_error()
{
    throw error();
    
}

BOOST_PYTHON_MODULE(exception_translator_ext)
{
  using namespace boost::python;
  register_exception_translator<error>(&translate);
  
  def("throw_error", throw_error);
}

