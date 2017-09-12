//-----------------------------------------------------------------------------
// boost mpl/test/vector_c.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/vector_c.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/size.hpp"
#include "boost/static_assert.hpp"
#include "boost/config.hpp"

namespace mpl = boost::mpl;

#if !defined(BOOST_MSVC) || (BOOST_MSVC > 1200)
void test_bool_list()
{
    typedef mpl::vector_c<bool,true>::type list1;
    typedef mpl::vector_c<bool,false>::type list2;

    BOOST_STATIC_ASSERT(mpl::front<list1>::type::value == true);
    BOOST_STATIC_ASSERT(mpl::front<list2>::type::value == false);
}
#endif

void test_int_list()
{
    typedef mpl::vector_c<int,-1>::type list1;
    typedef mpl::vector_c<int,0,1>::type list2;
    typedef mpl::vector_c<int,1,2,3>::type list3;

    BOOST_STATIC_ASSERT(mpl::size<list1>::type::value == 1);
    BOOST_STATIC_ASSERT(mpl::size<list2>::type::value == 2);
    BOOST_STATIC_ASSERT(mpl::size<list3>::type::value == 3);
    BOOST_STATIC_ASSERT(mpl::front<list1>::type::value == -1);
    BOOST_STATIC_ASSERT(mpl::front<list2>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::front<list3>::type::value == 1);
}

void test_unsigned_list()
{
    typedef mpl::vector_c<unsigned,0>::type list1;
    typedef mpl::vector_c<unsigned,1,2>::type list2;

    BOOST_STATIC_ASSERT(mpl::size<list1>::type::value == 1);
    BOOST_STATIC_ASSERT(mpl::size<list2>::type::value == 2);
    BOOST_STATIC_ASSERT(mpl::front<list1>::type::value == 0);
    BOOST_STATIC_ASSERT(mpl::front<list2>::type::value == 1);
}

int main()
{
    return 0;
}
