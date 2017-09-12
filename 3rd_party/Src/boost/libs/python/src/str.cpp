#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>

namespace boost { namespace python { namespace detail {

detail::new_reference str_base::call(object const& arg_)
{
    return (detail::new_reference)PyObject_CallFunction(
        (PyObject*)&PyString_Type, "(O)", 
        arg_.ptr());
} 

str_base::str_base()
    : object(detail::new_reference(PyString_FromString("")))
{}

str_base::str_base(const char* s)
    : object(detail::new_reference(PyString_FromString(s)))
{}

str_base::str_base(object_cref other)
    : object(str_base::call(other))
{}

namespace
{
  new_reference new_attr_reference(object const* obj, char const* name)
  {
      return new_reference(incref(object(obj->attr(name)).ptr()));
  }
}

#define BOOST_PYTHON_FORMAT_OBJECT(z, n, data) "O"
#define BOOST_PYTHON_OBJECT_PTR(z, n, data) , x##n .ptr()

#define BOOST_PYTHON_DEFINE_STR_METHOD(name, arity)                             \
str str_base:: name ( BOOST_PP_ENUM_PARAMS(arity, object_cref x) ) const        \
{                                                                               \
    return str(new_reference(                                                   \
       expect_non_null(                                                         \
           PyObject_CallMethod(                                                 \
               this->ptr(), #name,                                              \
              "(" BOOST_PP_REPEAT(arity, BOOST_PYTHON_FORMAT_OBJECT, _) ")"     \
               BOOST_PP_REPEAT_1(arity, BOOST_PYTHON_OBJECT_PTR, _)))));        \
}

BOOST_PYTHON_DEFINE_STR_METHOD(capitalize, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(center, 1)

long str_base::count(object_cref sub) const
{
    return extract<long>(this->attr("count")(sub));
}

long str_base::count(object_cref sub, object_cref start) const
{
    return extract<long>(this->attr("count")(sub,start));
}

long str_base::count(object_cref sub, object_cref start, object_cref end) const
{
    return extract<long>(this->attr("count")(sub,start,end));
}

object str_base::decode() const
{
    return this->attr("decode")();
}

object str_base::decode(object_cref encoding) const
{
    return this->attr("decode")(encoding);
}

object str_base::decode(object_cref encoding, object_cref errors) const
{
    return this->attr("decode")(encoding,errors);
}

object str_base::encode() const
{
    return this->attr("encode")();
}

object str_base::encode(object_cref encoding) const
{
    return this->attr("encode")(encoding);
}

object str_base::encode(object_cref encoding, object_cref errors) const
{
    return this->attr("encode")(encoding,errors);
}

bool str_base::endswith(object_cref suffix) const
{
    bool result = PyInt_AsLong(this->attr("endswith")(suffix).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

BOOST_PYTHON_DEFINE_STR_METHOD(expandtabs, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(expandtabs, 1)

long str_base::find(object_cref sub) const
{
    long result = PyInt_AsLong(this->attr("find")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::find(object_cref sub, object_cref start) const
{
    long result = PyInt_AsLong(this->attr("find")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::find(object_cref sub, object_cref start, object_cref end) const
{
    long result = PyInt_AsLong(this->attr("find")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub) const
{
    long result = PyInt_AsLong(this->attr("index")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub, object_cref start) const
{
    long result = PyInt_AsLong(this->attr("index")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub, object_cref start, object_cref end) const
{
    long result = PyInt_AsLong(this->attr("index")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isalnum() const
{
    bool result = PyInt_AsLong(this->attr("isalnum")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isalpha() const
{
    bool result = PyInt_AsLong(this->attr("isalpha")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isdigit() const
{
    bool result = PyInt_AsLong(this->attr("isdigit")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::islower() const
{
    bool result = PyInt_AsLong(this->attr("islower")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isspace() const
{
    bool result = PyInt_AsLong(this->attr("isspace")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::istitle() const
{
    bool result = PyInt_AsLong(this->attr("istitle")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isupper() const
{
    bool result = PyInt_AsLong(this->attr("isupper")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

BOOST_PYTHON_DEFINE_STR_METHOD(join, 1)
BOOST_PYTHON_DEFINE_STR_METHOD(ljust, 1)
BOOST_PYTHON_DEFINE_STR_METHOD(lower, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(lstrip, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(replace, 2)
BOOST_PYTHON_DEFINE_STR_METHOD(replace, 3)

long str_base::rfind(object_cref sub) const
{
    long result = PyInt_AsLong(this->attr("rfind")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rfind(object_cref sub, object_cref start) const
{
    long result = PyInt_AsLong(this->attr("rfind")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rfind(object_cref sub, object_cref start, object_cref end) const
{
    long result = PyInt_AsLong(this->attr("rfind")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub) const
{
    long result = PyInt_AsLong(this->attr("rindex")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub, object_cref start) const
{
    long result = PyInt_AsLong(this->attr("rindex")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub, object_cref start, object_cref end) const
{
    long result = PyInt_AsLong(this->attr("rindex")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

BOOST_PYTHON_DEFINE_STR_METHOD(rjust, 1)
BOOST_PYTHON_DEFINE_STR_METHOD(rstrip, 0)

list str_base::split() const
{
    return list(this->attr("split")());
}

list str_base::split(object_cref sep) const
{
    return list(this->attr("split")(sep));
}

list str_base::split(object_cref sep, object_cref maxsplit) const
{
    return list(this->attr("split")(sep,maxsplit));
}

list str_base::splitlines() const
{
    return list(this->attr("splitlines")());
}

list str_base::splitlines(object_cref keepends) const
{
    return list(this->attr("splitlines")(keepends));
}

bool str_base::startswith(object_cref prefix) const
{
    bool result = PyInt_AsLong(this->attr("startswith")(prefix).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::startswith(object_cref prefix, object_cref start) const
{
    bool result = PyInt_AsLong(this->attr("startswith")(prefix,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::startswith(object_cref prefix, object_cref start, object_cref end) const
{
    bool result = PyInt_AsLong(this->attr("startswith")(prefix,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

BOOST_PYTHON_DEFINE_STR_METHOD(strip, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(swapcase, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(title, 0)
BOOST_PYTHON_DEFINE_STR_METHOD(translate, 1)
BOOST_PYTHON_DEFINE_STR_METHOD(translate, 2)
BOOST_PYTHON_DEFINE_STR_METHOD(upper, 0)

}}}  // namespace boost::python
