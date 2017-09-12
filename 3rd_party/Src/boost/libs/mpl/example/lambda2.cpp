//-----------------------------------------------------------------------------
// boost mpl/example/lambda2.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2002
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/v2_1.hpp"

#include "boost/mpl/not.hpp"
#include "boost/mpl/or.hpp"
#include "boost/mpl/less.hpp"
#include "boost/mpl/equal_to.hpp"
#include "boost/mpl/multiplies.hpp"
#include "boost/mpl/prior.hpp"
#include "boost/mpl/find_if.hpp"
#include "boost/mpl/count_if.hpp"
#include "boost/mpl/filter_view.hpp"
#include "boost/mpl/sizeof.hpp"

#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;
using namespace mpl::placeholders;
using namespace mpl::v2_1;


typedef EVAL(apply_(lambda(is_same(_,_)), int, int)) r;

BOOST_STATIC_ASSERT(r::value);

typedef eval<
      count_if(
          list(int,char,long,int)
        , lambda(is_same(_,int))
        )
    >::type res;

BOOST_STATIC_ASSERT(res::value == 2);

typedef eval<
      find_if( 
          filter_view(
              list(int,float,char,long,double)
            , lambda(not_(is_float(_)))
            )
        , lambda( or_( 
              is_same(_,short)
            , less(sizeof_(_),sizeof_(int))
            ) )
        )
    >::type iter;

typedef eval< is_same(iter::type,char) >::type res2;
    
BOOST_STATIC_ASSERT(res2::value);



METAFUNCTION(factorial, (N)
    , if_( equal_to( N, int_<0> )
         , int_<1>
         , multiplies( N, factorial( prior(N) ) )
         )
    )

typedef EVAL(factorial(int_<1>)) fact;

//BOOST_STATIC_ASSERT(fact::value == 1);
