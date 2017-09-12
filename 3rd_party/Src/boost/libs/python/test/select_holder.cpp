// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/object/select_holder.hpp>
#include <boost/python/has_back_reference.hpp>
#include <boost/python/detail/not_specified.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/same_traits.hpp>
#include <boost/function/function0.hpp>
#include <memory>

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

struct BR {};

struct Base {};
struct Derived : Base {};

namespace boost { namespace python
{
  // specialization
  template <>
  struct has_back_reference<BR>
  {
      BOOST_STATIC_CONSTANT(bool, value = true);
  };
}} // namespace boost::python

template <class T, class U>
void assert_same(U* = 0, T* = 0)
{
    BOOST_TEST((boost::is_same<T,U>::value));
    BOOST_STATIC_ASSERT((boost::is_same<T,U>::value));
    
}

template <class T, class Held, class Holder>
void assert_holder(T* = 0, Held* = 0, Holder* = 0)
{
    assert_same<Holder>(
#if BOOST_WORKAROUND(__MWERKS__, <= 0x2407)
        boost::python::objects::select_holder<T,Held>::execute((Held*)0).get()
#else
        boost::python::objects::select_holder<T,Held>::type::get()
#endif 
        );
}

int test_main(int, char * [])
{
    using namespace boost::python::detail;
    using namespace boost::python::objects;

    assert_holder<Base,not_specified,value_holder<Base> >();

    assert_holder<BR,not_specified,value_holder_back_reference<BR,BR> >();
    assert_holder<Base,Base,value_holder<Base> >();
    assert_holder<BR,BR,value_holder_back_reference<BR,BR> >();

    assert_holder<Base,Derived
        ,value_holder_back_reference<Base,Derived> >();

    assert_holder<Base,std::auto_ptr<Base>
        ,pointer_holder<std::auto_ptr<Base>,Base> >();
    
    assert_holder<Base,std::auto_ptr<Derived>
        ,pointer_holder_back_reference<std::auto_ptr<Derived>,Base> >();

    assert_holder<BR,std::auto_ptr<BR>
        ,pointer_holder_back_reference<std::auto_ptr<BR>,BR> > ();

    return 0;
}

#if !defined(_WIN32) || defined(__GNUC__)
// This definition is needed for MinGW 2.95.2 and KCC on OSF for some
// reason, but will break other Win32 compilers.
namespace boost { namespace python
{
  bool handle_exception_impl(boost::function0<void>) { return false; }
}}
#endif 
