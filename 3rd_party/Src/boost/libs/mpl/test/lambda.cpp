//-----------------------------------------------------------------------------
// boost mpl/test/lambda.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/logical.hpp"
#include "boost/mpl/comparison.hpp"
#include "boost/mpl/lambda.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/bool.hpp"
#include "boost/mpl/sizeof.hpp"
#include "boost/mpl/apply.hpp"

#include "boost/type_traits/is_same.hpp"
#include "boost/type_traits/is_float.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;

struct my
{
    char a[100];
};

int main()
{
    using namespace mpl::placeholders;

    // !(x == char) && !(x == double) && x convertible to int || sizeof(x) > 8
    typedef mpl::lambda<
        mpl::or_<
              mpl::and_<
                    mpl::not_< boost::is_same<_1, char> >
                  , mpl::not_< boost::is_float<_1> >
                  >
            , mpl::greater< mpl::sizeof_<_1>, mpl::int_<8> >
            >
        >::type f1;

    BOOST_STATIC_ASSERT((!mpl::apply1<f1,char>::type::value));
    BOOST_STATIC_ASSERT((!mpl::apply1<f1,double>::type::value));
    BOOST_STATIC_ASSERT((mpl::apply1<f1,long>::type::value));
    BOOST_STATIC_ASSERT((mpl::apply1<f1,my>::type::value));

    // x == y || x == my || sizeof(x) == sizeof(y)
    typedef mpl::lambda<
        mpl::or_< 
              boost::is_same<_1, _2>
            , boost::is_same<_2, my>
            , mpl::equal_to< mpl::sizeof_<_1>, mpl::sizeof_<_2> >
            >
        >::type f2;

    BOOST_STATIC_ASSERT((!mpl::apply2<f2,double,char>::type::value));
    BOOST_STATIC_ASSERT((!mpl::apply2<f2,my,int>::type::value));
    BOOST_STATIC_ASSERT((!mpl::apply2<f2,my,char[99]>::type::value));
    BOOST_STATIC_ASSERT((mpl::apply2<f2,int,int>::type::value));
    BOOST_STATIC_ASSERT((mpl::apply2<f2,my,my>::type::value));
    BOOST_STATIC_ASSERT((mpl::apply2<f2,signed long, unsigned long>::type::value));

    // bind <-> lambda interaction
    typedef mpl::lambda< mpl::less<_1,_2> >::type pred;
    typedef mpl::bind2< pred, _1, mpl::int_<4> > f3;
    
    BOOST_STATIC_ASSERT((mpl::apply1< f3,mpl::int_<3> >::type::value));
          
    return 0;
}
