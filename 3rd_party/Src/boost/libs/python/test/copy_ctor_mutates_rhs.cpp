// Copyright David Abrahams 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/detail/copy_ctor_mutates_rhs.hpp>
#include <boost/static_assert.hpp>
#include <memory>
#include <string>

struct foo
{
    operator std::auto_ptr<int>&() const;
};

int main()
{
    using namespace boost::python::detail;
    BOOST_STATIC_ASSERT(!copy_ctor_mutates_rhs<int>::value);
    BOOST_STATIC_ASSERT(copy_ctor_mutates_rhs<std::auto_ptr<int> >::value);
    BOOST_STATIC_ASSERT(!copy_ctor_mutates_rhs<std::string>::value);
    BOOST_STATIC_ASSERT(!copy_ctor_mutates_rhs<foo>::value);
    return 0;
}
