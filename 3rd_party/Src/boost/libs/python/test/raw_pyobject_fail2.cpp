// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/converter/arg_to_python.hpp>

struct X : PyObject {};

int main()
{
    boost::python::converter::arg_to_python<X*> x(0);
    return 0;
}
