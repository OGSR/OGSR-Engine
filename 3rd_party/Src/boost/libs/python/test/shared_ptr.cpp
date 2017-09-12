// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/call_method.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/def.hpp>
#include <boost/shared_ptr.hpp>
#include "test_class.hpp"

#include <memory>

using namespace boost::python;
using boost::shared_ptr;

typedef test_class<> X;
typedef test_class<1> Y;

template <class T>
struct functions
{
    static int look(shared_ptr<T> const& x)
    {
        return (x.get()) ? x->value() : -1;
    }

    static void store(shared_ptr<T> x)
    {
        storage = x;
    }

    static void release_store()
    {
        store(shared_ptr<T>());
    }
    
    static void modify(shared_ptr<T>& x)
    {
        x.reset();
    }

    static shared_ptr<T> get() { return storage; }
        
    static int look_store()
    {
        return look(get());
    }
    
    static void expose()
    {
        def("look", &look);
        def("store", &store);
        def("modify", &modify); 
        def("look_store", &look_store); 
    }

    static shared_ptr<T> storage;
};

template <class T> shared_ptr<T> functions<T>::storage;

struct Z : test_class<2>
{
    Z(int x) : test_class<2>(x) {}
    virtual int v() { return this->value(); }
};

struct ZWrap : Z
{
    ZWrap(PyObject* self, int x)
        : Z(x), m_self(self) {}

    
    virtual int v() { return call_method<int>(m_self, "v"); }
    int default_v() { return Z::v(); }
    

    PyObject* m_self;
};

struct YY : Y
{
    YY(int n) : Y(n) {}
};

shared_ptr<Y> factory(int n)
{
    return shared_ptr<Y>(n < 42 ? new Y(n) : new YY(n));
}

static int stored_v() { return functions<Z>::get()->v(); }
static shared_ptr<Z> stored_z() { return functions<Z>::get(); }

BOOST_PYTHON_MODULE(shared_ptr_ext)
{
    class_<X, boost::noncopyable>("X", init<int>())
        .def("value", &X::value)
        ;

    def("factory", factory);
    
    functions<X>::expose();
    def("x_count", &X::count);
    def("x_release", &functions<X>::release_store);
    def("x_look_store", &functions<X>::look_store);
    
    class_<Y, boost::shared_ptr<Y> >("Y", init<int>())
        .def("value", &Y::value)
        ;
    
    class_<YY, bases<Y>, boost::noncopyable>("YY", init<int>())
        ;

    functions<Y>::expose();
    def("y_count", &Y::count);
    def("y_release", &functions<Y>::release_store);
    def("y_look_store", &functions<Y>::look_store);

    class_<Z, ZWrap>("Z", init<int>())
        .def("value", &Z::value)
        .def("v", &Z::v, &ZWrap::default_v)
        ;
    
    functions<Z>::expose();
    def("z_count", &Z::count);
    def("z_release", &functions<Z>::release_store);
    def("z_look_store", &functions<Z>::look_store);
    def("stored_z", &stored_z);
    def("stored_v", &stored_v);
}

#include "module_tail.cpp"

