// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/enum.hpp>
#include <boost/python/def.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#if BOOST_WORKAROUND(__MWERKS__, <= 0x2407)
# include <boost/type_traits/is_enum.hpp>
# include <boost/mpl/bool.hpp>
#endif 
using namespace boost::python;

enum color { red = 1, green = 2, blue = 4 };

#if BOOST_WORKAROUND(__MWERKS__, <= 0x2407)
namespace boost  // Pro7 has a hard time detecting enums
{
  template <> struct is_enum<color> : boost::mpl::true_ {};
}
#endif 

color identity_(color x) { return x; }

struct colorized {
    colorized() : x(red) {}
    color x;
};

BOOST_PYTHON_MODULE(enum_ext)
{
    enum_<color>("color")
        .value("red", red)
        .value("green", green)
        .value("blue", blue)
        .export_values()
        ;
    
    def("identity", identity_);

    class_<colorized>("colorized")
        .def_readwrite("x", &colorized::x)
        ;
}

#include "module_tail.cpp"
