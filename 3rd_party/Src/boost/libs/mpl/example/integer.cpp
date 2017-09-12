//-----------------------------------------------------------------------------
// boost mpl/example/integer.cpp source file
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

#include "boost/mpl/multiplies.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/lower_bound.hpp"
#include "boost/mpl/transform_view.hpp"
#include "boost/mpl/sizeof.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/identity.hpp"
#include "boost/mpl/base.hpp"
#include "boost/mpl/apply_if.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/mpl/begin_end.hpp"
#include "boost/mpl/assert_is_same.hpp"

namespace mpl = boost::mpl;
using namespace mpl::placeholders;

template< int bit_size >
class big_int
{
    // ...
};

template< int bit_size >
struct integer
{
    typedef mpl::list<char,short,int,long> builtins_;
    typedef typename mpl::base< typename mpl::lower_bound<
          mpl::transform_view< builtins_
            , mpl::multiplies< mpl::sizeof_<_1>, mpl::int_<8> >
            >
        , mpl::int_<bit_size>
        >::type >::type iter_;

    typedef typename mpl::end<builtins_>::type last_;
    typedef typename mpl::apply_if<
          boost::is_same<iter_,last_>
        , mpl::identity< big_int<bit_size> >
        , mpl::apply0<iter_>
        >::type type;
};

typedef integer<1>::type int1;
typedef integer<5>::type int5;
typedef integer<15>::type int15;
typedef integer<32>::type int32;
typedef integer<100>::type int100;

BOOST_MPL_ASSERT_IS_SAME(int1, char);
BOOST_MPL_ASSERT_IS_SAME(int5, char);
BOOST_MPL_ASSERT_IS_SAME(int15, short);
BOOST_MPL_ASSERT_IS_SAME(int32, int);
BOOST_MPL_ASSERT_IS_SAME(int100, big_int<100>);
