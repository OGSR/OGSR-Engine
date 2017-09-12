// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

// embedded_hello -- A simple Boost.Python embedding example -- by
// Dirk Gerrits

#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <stdexcept>

namespace python = boost::python;

// An abstract base class
class Base : public boost::noncopyable
{
public:
    virtual ~Base() {};

    virtual std::string hello() = 0;
};

// C++ derived class
class CppDerived : public Base
{
public:
    virtual ~CppDerived() {}
    
    std::string hello()
    {
        return "Hello from C++!";
    }
};

// Familiar Boost.Python wrapper class for Base
struct BaseWrap : public Base
{
    BaseWrap(PyObject* self_)
        : self(self_) {}

    std::string hello() { return python::call_method<std::string>(self, "hello"); }

    PyObject* self;
};

// Pack the Base class wrapper into a module
BOOST_PYTHON_MODULE(embedded_hello)
{
    python::class_<Base, BaseWrap, boost::noncopyable>("Base")
        ;

}


void test()
{
//- INITIALIZATION -----------------------------------------------------------//

    // Register the module with the interpreter
    if (PyImport_AppendInittab("embedded_hello", initembedded_hello) == -1)
        throw std::runtime_error("Failed to add embedded_hello to the interpreter's "
                                 "builtin modules");

    // Initialize the interpreter
    Py_Initialize();

    // Retrieve the main module
    python::handle<> main_module(
        python::borrowed(PyImport_AddModule("__main__")) );

    // Retrieve the main modules namespace
    python::handle<> main_namespace(
        python::borrowed(PyModule_GetDict(main_module.get())) );

    // Define the derived class in Python.
    // (You'll normally want to put this in a .py file.)
    python::handle<> result(
        PyRun_String(
        "from embedded_hello import *        \n"
        "class PythonDerived(Base):          \n"
        "    def hello(self):                \n"
        "        return 'Hello from Python!' \n", 
        Py_file_input, main_namespace.get(), main_namespace.get())
        );
    // Result is not needed
    result.reset();

    // Extract the raw Python object representing the just defined derived class
    python::handle<> class_ptr( 
        PyRun_String("PythonDerived\n", Py_eval_input, 
                     main_namespace.get(), main_namespace.get()) );

    // Wrap the raw Python object in a Boost.Python object
    python::object PythonDerived(class_ptr);

//- MAIN PROGRAM -------------------------------------------------------------//

    // Creating and using instances of the C++ class is as easy as always.
    CppDerived cpp;
    std::cout << cpp.hello() << std::endl;

    // But now creating and using instances of the Python class is almost 
    // as easy!
    python::object py_base = PythonDerived();
    Base& py = python::extract<Base&>(py_base)();
    std::cout << py.hello() << std::endl;
}

int main()
{
    if (python::handle_exception(test))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return 1;
    }
    return 0;
}
#include "module_tail.cpp"
