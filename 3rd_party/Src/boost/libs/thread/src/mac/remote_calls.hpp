// Copyright (C) 2001
// Mac Murrett
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Mac Murrett makes no representations
// about the suitability of this software for any purpose.  It is
// provided "as is" without express or implied warranty.
//
// See http://www.boost.org for most recent version including documentation.

#ifndef BOOST_REMOTE_CALLS_MJM012402_HPP
#define BOOST_REMOTE_CALLS_MJM012402_HPP


#include <boost/bind.hpp>

#include "remote_call_manager.hpp"
#include <boost/thread/detail/singleton.hpp>


// this file contains macros to generate functions with the signatures:
//  ReturnType st_remote_call([pascal] ReturnType (*pfnFunction)(
//                              [Argument1Type[, Argument2Type[...]]])
//                            [, Argument1Type oArgument1[, Argument2Type oArgument2[...]]])
// and
//  ReturnType dt_remote_call([pascal] ReturnType (*pfnFunction)(
//                              [Argument1Type[, Argument2Type[...]]])
//                            [, Argument1Type oArgument1[, Argument2Type oArgument2[...]]])
// in other words, identical to the function pointer versions of boost::bind, but
//  with the return type returned.  The purpose of these functions is to be able to
//  request that a function be called at system task time or deferred task time, then
//  sleep until it is called, and finally get back its return value.

#define BOOST_REMOTE_CALL_CLASS_LIST_0
#define BOOST_REMOTE_CALL_CLASS_LIST_1 BOOST_REMOTE_CALL_CLASS_LIST_0, class A1
#define BOOST_REMOTE_CALL_CLASS_LIST_2 BOOST_REMOTE_CALL_CLASS_LIST_1, class A2
#define BOOST_REMOTE_CALL_CLASS_LIST_3 BOOST_REMOTE_CALL_CLASS_LIST_2, class A3
#define BOOST_REMOTE_CALL_CLASS_LIST_4 BOOST_REMOTE_CALL_CLASS_LIST_3, class A4
#define BOOST_REMOTE_CALL_CLASS_LIST_5 BOOST_REMOTE_CALL_CLASS_LIST_4, class A5
#define BOOST_REMOTE_CALL_CLASS_LIST_6 BOOST_REMOTE_CALL_CLASS_LIST_5, class A6
#define BOOST_REMOTE_CALL_CLASS_LIST_7 BOOST_REMOTE_CALL_CLASS_LIST_6, class A7
#define BOOST_REMOTE_CALL_CLASS_LIST_8 BOOST_REMOTE_CALL_CLASS_LIST_7, class A8
#define BOOST_REMOTE_CALL_CLASS_LIST_9 BOOST_REMOTE_CALL_CLASS_LIST_8, class A9


#define BOOST_REMOTE_CALL_ARGUMENT_LIST_0
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_1 BOOST_REMOTE_CALL_ARGUMENT_LIST_0  A1 oA1
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_2 BOOST_REMOTE_CALL_ARGUMENT_LIST_1, A2 oA2
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_3 BOOST_REMOTE_CALL_ARGUMENT_LIST_2, A3 oA3
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_4 BOOST_REMOTE_CALL_ARGUMENT_LIST_3, A4 oA4
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_5 BOOST_REMOTE_CALL_ARGUMENT_LIST_4, A5 oA5
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_6 BOOST_REMOTE_CALL_ARGUMENT_LIST_5, A6 oA6
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_7 BOOST_REMOTE_CALL_ARGUMENT_LIST_6, A7 oA7
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_8 BOOST_REMOTE_CALL_ARGUMENT_LIST_7, A8 oA8
#define BOOST_REMOTE_CALL_ARGUMENT_LIST_9 BOOST_REMOTE_CALL_ARGUMENT_LIST_8, A9 oA9


#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_0
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_1 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_0, oA1
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_2 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_1, oA2
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_3 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_2, oA3
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_4 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_3, oA4
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_5 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_4, oA5
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_6 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_5, oA6
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_7 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_6, oA7
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_8 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_7, oA8
#define BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_9 BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_8, oA9


#define BOOST_REMOTE_CALL_COMMA_0
#define BOOST_REMOTE_CALL_COMMA_1 ,
#define BOOST_REMOTE_CALL_COMMA_2 ,
#define BOOST_REMOTE_CALL_COMMA_3 ,
#define BOOST_REMOTE_CALL_COMMA_4 ,
#define BOOST_REMOTE_CALL_COMMA_5 ,
#define BOOST_REMOTE_CALL_COMMA_6 ,
#define BOOST_REMOTE_CALL_COMMA_7 ,
#define BOOST_REMOTE_CALL_COMMA_8 ,
#define BOOST_REMOTE_CALL_COMMA_9 ,


// this is the macro that ties it all together.  From here, we generate all forms of
//  dt_remote_call and st_remote_call.

#define BOOST_REMOTE_CALL(context, stack, n)                                        \
template<class R BOOST_REMOTE_CALL_CLASS_LIST_ ## n>                                \
inline R context ## _remote_call(stack R (*pfnF)(                                   \
                                      BOOST_REMOTE_CALL_ARGUMENT_LIST_ ## n)        \
                                      BOOST_REMOTE_CALL_COMMA_ ## n                 \
                                  BOOST_REMOTE_CALL_ARGUMENT_LIST_ ## n)            \
{                                                                                   \
    using ::boost::detail::thread::singleton;                                       \
    using detail::remote_call_manager;                                              \
    function<R> oFunc(bind(pfnF BOOST_REMOTE_CALL_FUNCTION_ARGUMENT_LIST_ ## n));   \
    remote_call_manager &rManager(singleton<remote_call_manager>::instance());      \
    return(rManager.execute_at_ ## context(oFunc));                                 \
}


namespace boost {

namespace threads {

namespace mac {


BOOST_REMOTE_CALL(st, , 0)
BOOST_REMOTE_CALL(st, , 1)
BOOST_REMOTE_CALL(st, , 2)
BOOST_REMOTE_CALL(st, , 3)
BOOST_REMOTE_CALL(st, , 4)
BOOST_REMOTE_CALL(st, , 5)
BOOST_REMOTE_CALL(st, , 6)
BOOST_REMOTE_CALL(st, , 7)
BOOST_REMOTE_CALL(st, , 8)
BOOST_REMOTE_CALL(st, , 9)

BOOST_REMOTE_CALL(dt, , 0)
BOOST_REMOTE_CALL(dt, , 1)
BOOST_REMOTE_CALL(dt, , 2)
BOOST_REMOTE_CALL(dt, , 3)
BOOST_REMOTE_CALL(dt, , 4)
BOOST_REMOTE_CALL(dt, , 5)
BOOST_REMOTE_CALL(dt, , 6)
BOOST_REMOTE_CALL(dt, , 7)
BOOST_REMOTE_CALL(dt, , 8)
BOOST_REMOTE_CALL(dt, , 9)


BOOST_REMOTE_CALL(st, pascal, 0)
BOOST_REMOTE_CALL(st, pascal, 1)
BOOST_REMOTE_CALL(st, pascal, 2)
BOOST_REMOTE_CALL(st, pascal, 3)
BOOST_REMOTE_CALL(st, pascal, 4)
BOOST_REMOTE_CALL(st, pascal, 5)
BOOST_REMOTE_CALL(st, pascal, 6)
BOOST_REMOTE_CALL(st, pascal, 7)
BOOST_REMOTE_CALL(st, pascal, 8)
BOOST_REMOTE_CALL(st, pascal, 9)

BOOST_REMOTE_CALL(dt, pascal, 0)
BOOST_REMOTE_CALL(dt, pascal, 1)
BOOST_REMOTE_CALL(dt, pascal, 2)
BOOST_REMOTE_CALL(dt, pascal, 3)
BOOST_REMOTE_CALL(dt, pascal, 4)
BOOST_REMOTE_CALL(dt, pascal, 5)
BOOST_REMOTE_CALL(dt, pascal, 6)
BOOST_REMOTE_CALL(dt, pascal, 7)
BOOST_REMOTE_CALL(dt, pascal, 8)
BOOST_REMOTE_CALL(dt, pascal, 9)


} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_REMOTE_CALLS_MJM012402_HPP
