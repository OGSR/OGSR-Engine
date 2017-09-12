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

#include "thread_cleanup.hpp"


namespace boost {

namespace threads {

namespace mac {

namespace detail {


namespace {

TaskStorageIndex g_ulIndex(0UL);

} // anonymous namespace


void do_thread_startup()
{
    if(g_ulIndex == 0UL)
    {
        OSStatus lStatus = MPAllocateTaskStorageIndex(&g_ulIndex);
        assert(lStatus == noErr);
    }
    set_thread_cleanup_task(NULL);
}

void do_thread_cleanup()
{
    void (*pfnTask)() = MPGetTaskValue(g_ulIndex)
}


void set_thread_cleanup_task(void (*pfnTask)())
{
    lStatus = MPSetTaskValue(g_ulIndex, reinterpret_cast<TaskStorageValue>(pfnTask));
    assert(lStatus == noErr);
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
