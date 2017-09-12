// Copyright David Hawkes 2002.
// Permission is hereby granted to copy, use and modify this software
// for any purpose, including commercial distribution, provided this
// copyright notice is not removed. No warranty WHATSOEVER is provided with this
// software. Any user(s) accepts this software "as is" and as such they will not
// bind the author(s) to any claim of suitabilty for any purpose.

// embed_test.cpp : substantial test of embedding python in c++ using boost

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/call_method.hpp>
#include <boost/python/call.hpp>
#include <boost/python/object.hpp>
#include <boost/python/py_interface.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;


// The following macros are for our convenience and coding expediency...
// There is no particular recomendation that they be used elsewhere
// as they are not feature complete and are just sufficient to suport
// the code here


#define DEF(fn) def(#fn, fn)
#define DEF_C(c, fn) def(#fn, &c::fn)
#define DEF_C_CB(c, fn) def(#fn, &c##_callback::base_##fn)

#define CLASS(c) class_<c, shared_ptr<c> >(#c)
#define CLASS_CB(c) class_<c, shared_ptr<c##_callback>, noncopyable >(#c)

#define START_CALLBACK_CLASS(c)                     \
class c##_callback : public c                       \
{                                                   \
    typedef c __base__;                             \
public:                                             \
    c##_callback(PyObject* self) : m_self(self) {}  \
private:                                            \
    PyObject* m_self;                               \
public:

#define END_CALLBACK_CLASS  };


#define CALLBACK_MEMBER0(fn, rtn)                   \
rtn fn() { return call_method<rtn>(m_self, #fn); }  \
rtn base_##fn() { return __base__::fn(); }


#define CALLBACK_MEMBER0C(fn, rtn)                          \
rtn fn() const { return call_method<rtn>(m_self, #fn); }    \
rtn base_##fn() const { return __base__::fn(); }
// End of convenience macros

// useful support classes
template<class T>
struct class_object : public object
{
    typedef object base;
    class_object() : m_class_ptr(NULL) {}
    class_object(object const& o) : base(o) { init(); }
    class_object& operator=(object const& o)
    {
        base::operator=(o);
        init();
        return *this;
    }
    T* operator->() const { return m_class_ptr; }
    T& operator*() const { return *m_class_ptr; }
private:
    void init()
    {
        m_class_ptr = arg_from_python<T*>(ptr())(ptr());
    }
    T* m_class_ptr;
};

template<class T>
struct item_object : public object
{
    typedef object base;
    item_object() {}
    item_object(object const& o) : base(o), m_class(arg_from_python<T>(ptr())(ptr())) {}
    item_object& operator=(object const& o)
    {
        base::operator=(o);
        init();
        return *this;
    }
    operator T() { return m_class; }
private:
    void init()
    {
        m_class = arg_from_python<T>(ptr())(ptr());
    }
    T m_class;
};
// end of useful support classes

// pass our args in this struct
struct main_args {
    main_args(int _argc, char* _argv[]) : argc(_argc), argv(_argv) {}
    int argc;
    char** argv;
};
int python_main(main_args const &ma);

// python module init
BOOST_PYTHON_MODULE(python_main)
{
    DEF(python_main);
    CLASS(main_args);
}

// sub module tests
namespace sm {

int test_func() { return 7; }

BOOST_PYTHON_MODULE(sm_test)
{
    // define a submodule
    boost::python::module(".sm");
    // define a 2nd submodule
    boost::python::module(".sm.sm2");
    // define a test function to appear in 2nd submodule
    DEF(test_func);
}

// sub-module tests
int test()
{
    api::run_simple_string("import sm_test");
    if(api::call_statement("_0 = bpl_test('sub modules', sm_test.sm.sm2.test_func, _1)", test_func()))
        return 1;
	return 0;
}

}

// sub class tests
namespace sc {

class c1 {
public:
    c1() {}
    class c2 {
    public:
        c2() : n(2) {}
        int n;
    };
    c2 t;
};

c1::c2 test_func() {
    return c1().t;
}

BOOST_PYTHON_MODULE(sc_test)
{
    class_<c1::c2>("c1.c2")
        .def_init()
        .def_readwrite("n", &c1::c2::n);
    CLASS(c1)
        .def_init()
        .def_readwrite("t", &c1::t);
    DEF(test_func);
}

// sub-class tests
int test()
{
    api::run_simple_string("import sc_test");
    if(api::call_statement("_0 = bpl_test('sub classes', lambda : sc_test.c1.c2().n, _1.n)", test_func()))
        return 1;
    if(api::call_statement("_0 = bpl_test('sub classes', lambda : sc_test.c1().t.n, _1.n)", test_func()))
        return 1;
	return 0;
}

}

// new main that will have a python execution frame
int main(int argc, char* argv[])
{
    // default return value;
    int rtn = 0;
    // define all the built-in modules used
    PyImport_AppendInittab("python_main", initpython_main);
    PyImport_AppendInittab("sm_test", sm::initsm_test);
    PyImport_AppendInittab("sc_test", sc::initsc_test);
    // initialize python
    Py_Initialize();
    // start a new block so that any objects are released prior to finalizing
    {
        // import our main module - this will also initialise boost
        api::run_simple_string("import python_main");
        // We call back here so we have a proper python execution frame to work with
        item_object<int> o_rtn(api::call_statement("_0 = python_main.python_main(_1)", main_args(argc, argv)));
        rtn = o_rtn;
    }
    // clean up
    Py_Finalize();
	return rtn;
}

char *bpl_test = 
"def bpl_test(name, func, result):\n"
"    print 'testing %s...' % name\n"
"    if func() != result:\n"
"        print 'failed'\n"
"        return 1\n"
"    else:\n"
"        print 'OK'\n"
"        return 0\n";


int python_main(main_args const &ma)
{
    api::print("running...\n");
    api::call_statement(bpl_test);
    if(sm::test())
        return 1;
    if(sc::test())
        return 1;
    return 0;
}
