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

#include "init.hpp"

#include "remote_call_manager.hpp"


#include <boost/thread/detail/singleton.hpp>

#include <Multiprocessing.h>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


namespace {

// force these to get called by the end of static initialization time.
static bool g_bInitialized = (thread_init() && create_singletons());

}


bool thread_init()
{
    static bool bResult = MPLibraryIsLoaded();

    return(bResult);
}

bool create_singletons()
{
    using ::boost::detail::thread::singleton;

    singleton<remote_call_manager>::instance();

    return(true);
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
