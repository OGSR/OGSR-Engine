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

#include "dt_scheduler.hpp"

#include "ot_context.hpp"


#include <boost/thread/detail/singleton.hpp>

#include <OpenTransportProtocol.h>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


const OTTimeout k_ulTimerTaskDelay = 1UL;


dt_scheduler::dt_scheduler():
    m_bReschedule(false),
    m_uppTask(NULL),
    m_lTask(0UL)
{
    using ::boost::detail::thread::singleton;

    ot_context &rContext(singleton<ot_context>::instance());

    m_uppTask = NewOTProcessUPP(task_entry);
    m_lTask = OTCreateTimerTaskInContext(m_uppTask, this, rContext.get_context());
}

dt_scheduler::~dt_scheduler()
{
    OTDestroyTimerTask(m_lTask);
    m_lTask = 0UL;
    DisposeOTProcessUPP(m_uppTask);
    m_uppTask = NULL;
}


void dt_scheduler::start_polling()
{
    m_bReschedule = true;
    schedule_task();
}

void dt_scheduler::stop_polling()
{
    m_bReschedule = false;
}


void dt_scheduler::schedule_task()
{
    if(m_bReschedule)
    {
        OTScheduleTimerTask(m_lTask, k_ulTimerTaskDelay);
    }
}


/*static*/ pascal void dt_scheduler::task_entry(void *pRefCon)
{
    dt_scheduler *pThis = reinterpret_cast<dt_scheduler *>(pRefCon);
    assert(pThis != NULL);
    pThis->task();
}

void dt_scheduler::task()
{
    periodic_function();
    schedule_task();
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
