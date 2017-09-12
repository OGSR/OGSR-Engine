//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]

char const* greet()
{
   return "hello, world";
}

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(hello)
{
    def("greet", greet);
}

