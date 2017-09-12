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

#ifndef BOOST_EXECUTION_CONTEXT_MJM012402_HPP
#define BOOST_EXECUTION_CONTEXT_MJM012402_HPP

namespace boost {

namespace threads {

namespace mac {


// utility functions for figuring out what context your code is executing in.
//    Bear in mind that at_mp and in_blue are the only functions guarenteed by
//    Apple to work.  There is simply no way of being sure that you will not get
//    false readings about task level at interrupt time in blue.

typedef enum {
    k_eExecutionContextSystemTask,
    k_eExecutionContextDeferredTask,
    k_eExecutionContextMPTask,
    k_eExecutionContextOther
} execution_context_t;

execution_context_t execution_context();

inline bool at_st()
    {    return(execution_context() == k_eExecutionContextSystemTask);    }

inline bool at_mp()
    {    return(execution_context() == k_eExecutionContextMPTask);        }
inline bool in_blue()
    {    return(!at_mp());                                                }


} // namespace mac

} // namespace threads

} // namespace boost

#endif // BOOST_EXECUTION_CONTEXT_MJM012402_HPP
