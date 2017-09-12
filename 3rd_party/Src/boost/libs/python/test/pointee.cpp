// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/pointee.hpp>
#include <boost/type_traits/same_traits.hpp>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

struct A;

int main()
{
    BOOST_STATIC_ASSERT(
        (boost::is_same<
                boost::python::pointee<std::auto_ptr<char**> >::type
                , char**
         >::value));
    
    BOOST_STATIC_ASSERT(
        (boost::is_same<
             boost::python::pointee<boost::shared_ptr<A> >::type
             , A>::value));

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    BOOST_STATIC_ASSERT(
        (boost::is_same<
                boost::python::pointee<char*>::type
                , char
         >::value));
#endif 
    return 0;
}
