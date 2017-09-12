// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/numeric.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

using namespace boost::python;

// See if we can invoke array() from C++
object new_array()
{
    return numeric::array(
        make_tuple(
            make_tuple(1,2,3)
          , make_tuple(4,5,6)
          , make_tuple(7,8,9)
            )
        );
}

// test argument conversion
void take_array(numeric::array x)
{
}

// A separate function to invoke the info() member. Must happen
// outside any doctests since this prints directly to stdout and the
// result text includes the address of the 'self' array.
void info(numeric::array const& z)
{
    z.info();
}

// Tests which work on both Numeric and numarray array objects. Of
// course all of the operators "just work" since numeric::array
// inherits that behavior from object.
void exercise(numeric::array& y, object check)
{
    y[make_tuple(2,1)] = 3;
    check(y);
    check(y.astype('D'));
    check(y.copy());
    check(y.typecode());
}

// numarray-specific tests.  check is a callable object which we can
// use to record intermediate results, which are later compared with
// the results of corresponding python operations.
void exercise_numarray(numeric::array& y, object check)
{
    check(y.astype());
    
    check(y.argmax());
    check(y.argmax(0));
    
    check(y.argmin());
    check(y.argmin(0));
    
    check(y.argsort());
    check(y.argsort(1));

    y.byteswap();
    check(y);
    
    check(y.diagonal());
    check(y.diagonal(1));
    check(y.diagonal(0, 1));
    check(y.diagonal(0, 1, 0));

    check(y.is_c_array());
    check(y.isbyteswapped());

    check(y.trace());
    check(y.trace(1));
    check(y.trace(0, 1));
    check(y.trace(0, 1, 0));

    check(y.new_('D'));
    y.sort();
    check(y);
    check(y.type());

    check(y.factory(make_tuple(1.2, 3.4)));
    check(y.factory(make_tuple(1.2, 3.4), "Double"));
    check(y.factory(make_tuple(1.2, 3.4), "Double", make_tuple(1,2,1)));
    check(y.factory(make_tuple(1.2, 3.4), "Double", make_tuple(2,1,1), false));
    check(y.factory(make_tuple(1.2, 3.4), "Double", make_tuple(2), true, true));
}

BOOST_PYTHON_MODULE(numpy_ext)
{
    def("new_array", new_array);
    def("take_array", take_array);
    def("exercise", exercise);
    def("exercise_numarray", exercise_numarray);
    def("set_module_and_type", &numeric::array::set_module_and_type);
    def("info", info);
}

#include "module_tail.cpp"
