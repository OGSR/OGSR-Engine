//-----------------------------------------------------------------------------
// boost mpl/test/apply.cpp source file
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

#include "boost/mpl/apply.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/mpl/limits/arity.hpp"
#include "boost/mpl/aux_/preprocessor/params.hpp"
#include "boost/mpl/aux_/preprocessor/enum.hpp"

#include "boost/preprocessor/repeat.hpp"
#include "boost/preprocessor/comma_if.hpp"
#include "boost/preprocessor/dec.hpp"
#include "boost/preprocessor/if.hpp"
#include "boost/preprocessor/cat.hpp"

namespace mpl = boost::mpl;

#define APPLY_0_FUNC_DEF(i) \
    struct f0 { typedef char type; }; \
/**/

#define APPLY_N_FUNC_DEF(i) \
    struct first##i \
    { \
        template< BOOST_MPL_PP_PARAMS(i, typename U) > \
        struct apply { typedef U1 type; }; \
    }; \
    \
    struct last##i \
    { \
        template< BOOST_MPL_PP_PARAMS(i, typename U) > \
        struct apply { typedef BOOST_PP_CAT(U,i) type; }; \
    }; \
/**/

#define APPLY_FUNC_DEF(z, i, unused) \
    BOOST_PP_IF( \
          i \
        , APPLY_N_FUNC_DEF \
        , APPLY_0_FUNC_DEF \
        )(i) \
/**/

namespace aux {
BOOST_PP_REPEAT(
      BOOST_MPL_METAFUNCTION_MAX_ARITY
    , APPLY_FUNC_DEF
    , unused
    )
}

#define APPLY_0_TEST(i, apply_) \
    typedef mpl::apply_<aux::f##i>::type t; \
    { BOOST_MPL_ASSERT_IS_SAME(t, char); } \
/**/

#define APPLY_N_TEST(i, apply_) \
    typedef mpl::apply_< \
          aux::first##i \
        , char \
        BOOST_PP_COMMA_IF(BOOST_PP_DEC(i)) \
        BOOST_MPL_PP_ENUM(BOOST_PP_DEC(i), int) \
        >::type t1; \
    \
    typedef mpl::apply_< \
          aux::last##i \
        , BOOST_MPL_PP_ENUM(BOOST_PP_DEC(i), int) \
        BOOST_PP_COMMA_IF(BOOST_PP_DEC(i)) char \
        >::type t2; \
    { BOOST_MPL_ASSERT_IS_SAME(t1, char); } \
    { BOOST_MPL_ASSERT_IS_SAME(t2, char); } \
/**/

#define APPLY_TEST(z, i, APPLY_NAME) \
    BOOST_PP_IF( \
          i \
        , APPLY_N_TEST \
        , APPLY_0_TEST \
        )(i, APPLY_NAME(i)) \
/**/

#define MAKE_APPLY_N_NAME(i) apply##i
#define MAKE_APPLY_NAME(i) apply

int main()
{
    BOOST_PP_REPEAT(
          BOOST_MPL_METAFUNCTION_MAX_ARITY
        , APPLY_TEST
        , MAKE_APPLY_N_NAME
        )

#if defined(BOOST_MPL_HAS_APPLY)
    BOOST_PP_REPEAT(
          BOOST_MPL_METAFUNCTION_MAX_ARITY
        , APPLY_TEST
        , MAKE_APPLY_NAME
        )
#endif
    return 0;
}
