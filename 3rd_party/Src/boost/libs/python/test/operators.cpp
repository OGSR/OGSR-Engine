// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/operators.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <string>
#include "test_class.hpp"
#if __GNUC__ != 2
# include <ostream>
#else
# include <ostream.h>
#endif

// Just use math.h here; trying to use std::pow() causes too much
// trouble for non-conforming compilers and libraries.
#include <math.h>

using namespace boost::python;

struct X : test_class<>
{
    typedef test_class<> base_t;
    
    X(int x) : base_t(x) {}
    X const operator+(X const& r) const { return X(value() + r.value()); }
};

X operator-(X const& l, X const& r) { return X(l.value() - r.value()); }
X operator-(int l, X const& r) { return X(l - r.value()); }
X operator-(X const& l, int r) { return X(l.value() - r); }

X operator-(X const& x) { return X(-x.value()); }

X& operator-=(X& l, X const& r) { l.set(l.value() - r.value()); return l; }

bool operator<(X const& x, X const& y) { return x.value() < y.value(); }
bool operator<(X const& x, int y) { return x.value() < y; }
bool operator<(int x, X const& y) { return x < y.value(); }

X abs(X x) { return X(x.value() < 0 ? -x.value() : x.value()); }

X pow(X x, int y)
{
    return X(int(pow(double(x.value()), double(y))));
}

X pow(X x, X y)
{
    return X(int(pow(double(x.value()), double(y.value()))));
}

int pow(int x, X y)
{
    return int(pow(double(x), double(y.value())));
}

std::ostream& operator<<(std::ostream& s, X const& x)
{
    return s << x.value();
}

BOOST_PYTHON_MODULE(operators_ext)
{
    class_<X>("X", init<int>())
        .def("value", &X::value)
        .def(self + self)
        .def(self - self)
        .def(self - int())
        .def(other<int>() - self)
        .def(-self)
        .def(self < other<int>())
        .def(self < self)
        .def(1 < self)
        .def(self -= self)
        .def(abs(self))
        .def(str(self))
            
        .def(pow(self,self))
        .def(pow(self,int()))
        .def(pow(int(),self))
        ;

    class_<test_class<1> >("Z", init<int>())
        .def(int_(self))
        .def(float_(self))
        .def(complex_(self))
        ;
}

#include "module_tail.cpp"
