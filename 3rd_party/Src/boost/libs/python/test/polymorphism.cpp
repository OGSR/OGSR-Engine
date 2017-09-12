// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/call_method.hpp>
#include <boost/python/def.hpp>
#include <boost/utility.hpp>

using namespace boost::python;

struct Callback
{
    Callback(PyObject* o) : mSelf(o) {}
    PyObject* mSelf;
};

struct A
{
    virtual ~A(){}
    virtual std::string f() { return "A::f()"; }
};

struct ACallback :  A,  Callback
{
    ACallback (PyObject* self) : Callback(self) {}
    
    
    std::string f()
    {
        return call_method<std::string>(mSelf, "f");
    }
    
    std::string default_f()
    {
        return A::f();
    }
};

struct B : A
{
    virtual std::string f() { return "B::f()"; }
};

struct C : A
{
    virtual std::string f() { return "C::f()"; }
};

A& getBCppObj ()
{
    static B b;
    return b;
}

std::string call_f(A& a) { return a.f(); }

A* factory(unsigned choice)
{
    switch (choice % 3)
    {
    case 0: return new A;
        break;
    case 1: return new B;
        break;
    default: return new C;
        break;
    }
}

C& getCCppObj ()
{
    static C c;
    return c;
}

BOOST_PYTHON_MODULE_INIT(polymorphism_ext)
{
    class_<A,boost::noncopyable,ACallback>("A")
        .def("f", &A::f, &ACallback::default_f)
        ;
    
    def("getBCppObj", getBCppObj, return_value_policy<reference_existing_object>());

    class_<C,bases<A>,boost::noncopyable>("C")
        .def("f", &C::f)
        ;
    
    def("getCCppObj", getCCppObj, return_value_policy<reference_existing_object>());

    def("factory", factory, return_value_policy<manage_new_object>());

    def("call_f", call_f);
}

//#include "module_tail.cpp"
