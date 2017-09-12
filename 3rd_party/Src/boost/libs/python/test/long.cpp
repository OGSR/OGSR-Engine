// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/long.hpp>
#include <cassert>

using namespace boost::python;

object new_long()
{
    return long_();
}

long_ longify(object x)
{
    return long_(x);
}

object longify_string(char const* s)
{
    return long_(s);
}

char const* is_long1(long_& x)
{
    long_ y = x;
    x += 50;
    assert(x == y + 50);
    return "yes";
}

int is_long2(char const*)
{
    return 0;
}

BOOST_PYTHON_MODULE(long_ext)
{
    def("new_long", new_long);
    def("longify", longify);
    def("longify_string", longify_string);
    def("is_long", is_long1);
    def("is_long", is_long2);
        ;
}

