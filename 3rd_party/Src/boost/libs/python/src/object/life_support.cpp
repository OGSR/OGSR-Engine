// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/object/life_support.hpp>
#include <boost/python/detail/none.hpp>
#include <boost/python/refcount.hpp>

namespace boost { namespace python { namespace objects { 

struct life_support
{
    PyObject_HEAD
    PyObject* patient;
};

extern "C"
{
    static void
    life_support_dealloc(PyObject* self)
    {
        Py_XDECREF(((life_support*)self)->patient);
        self->ob_type->tp_free(self);
    }

    static PyObject *
    life_support_call(PyObject *self, PyObject *arg, PyObject *kw)
    {
        // Let the patient die now
        Py_XDECREF(((life_support*)self)->patient);
        ((life_support*)self)->patient = 0;
        // Let the weak reference die. This probably kills us.
        Py_XDECREF(PyTuple_GET_ITEM(arg, 0));
        return detail::none();
    }
}

PyTypeObject life_support_type = {
    PyObject_HEAD_INIT(0)//(&PyType_Type)
    0,
    "Boost.Python.life_support",
    sizeof(life_support),
    0,
    life_support_dealloc,               /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    0, //(reprfunc)func_repr,                   /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    life_support_call,                  /* tp_call */
    0,                                  /* tp_str */
    0, // PyObject_GenericGetAttr,            /* tp_getattro */
    0, // PyObject_GenericSetAttr,            /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT /* | Py_TPFLAGS_HAVE_GC */,/* tp_flags */
    0,                                  /* tp_doc */
    0, // (traverseproc)func_traverse,          /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0, //offsetof(PyLife_SupportObject, func_weakreflist), /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0, // func_memberlist,                      /* tp_members */
    0, //func_getsetlist,                       /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0, //offsetof(PyLife_SupportObject, func_dict),      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    0,
    0                                       /* tp_new */
};

PyObject* make_nurse_and_patient(PyObject* nurse, PyObject* patient)
{
    if (nurse == Py_None)
        return incref(nurse);
    
    if (life_support_type.ob_type == 0)
    {
        life_support_type.ob_type = &PyType_Type;
        PyType_Ready(&life_support_type);
    }
    
    life_support* system = PyObject_New(life_support, &life_support_type);
    if (!system)
        return 0;

    system->patient = 0;
    
    // We're going to leak this reference, but don't worry; the
    // life_support system decrements it when the nurse dies.
    PyObject* weakref = PyWeakref_NewRef(nurse, (PyObject*)system);
    if (!weakref)
    {
        Py_XDECREF(system);
        return 0;
    }
    
    system->patient = patient;
    Py_XINCREF(patient); // hang on to the patient until death
    return weakref;
}

}}} // namespace boost::python::objects
