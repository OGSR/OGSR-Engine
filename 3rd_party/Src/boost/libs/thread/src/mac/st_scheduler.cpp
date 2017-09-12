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

#include "st_scheduler.hpp"

#include <cassert>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


#if TARGET_CARBON

st_scheduler::st_scheduler():
    m_uppTask(NULL),
    m_pTimer(NULL)
{
    m_uppTask = NewEventLoopTimerUPP(task_entry);
// TODO - throw on error
    assert(m_uppTask != NULL);
}

st_scheduler::~st_scheduler()
{
    DisposeEventLoopTimerUPP(m_uppTask);
    m_uppTask = NULL;
}


void st_scheduler::start_polling()
{
    assert(m_pTimer == NULL);
    OSStatus lStatus = InstallEventLoopTimer(GetMainEventLoop(),
                                                0 * kEventDurationSecond,
                                                kEventDurationMillisecond,
                                                m_uppTask,
                                                this,
                                                &m_pTimer);
// TODO - throw on error
    assert(lStatus == noErr);
}

void st_scheduler::stop_polling()
{
    assert(m_pTimer != NULL);
    OSStatus lStatus = RemoveEventLoopTimer(m_pTimer);
    assert(lStatus == noErr);
    m_pTimer = NULL;
}


/*static*/ pascal void st_scheduler::task_entry(EventLoopTimerRef /*pTimer*/, void *pRefCon)
{
    st_scheduler *pThis = reinterpret_cast<st_scheduler *>(pRefCon);
    assert(pThis != NULL);
    pThis->task();
}

void st_scheduler::task()
{
    periodic_function();
}

#else
#    error st_scheduler unimplemented!
#endif


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
