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

#ifndef BOOST_DT_SCHEDULER_MJM012402_HPP
#define BOOST_DT_SCHEDULER_MJM012402_HPP


#include "periodical.hpp"

#include <OpenTransport.h>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// class dt_scheduler calls its pure-virtual periodic_function method periodically at
//    deferred task time.  This is generally 1kHz under Mac OS 9.

class dt_scheduler
{
  public:
    dt_scheduler();
    virtual ~dt_scheduler();

  protected:
    void start_polling();
    void stop_polling();

  private:
    virtual void periodic_function() = 0;

  private:
    void schedule_task();
    static pascal void task_entry(void *pRefCon);
    void task();

  private:
    bool m_bReschedule;
    OTProcessUPP m_uppTask;
    long m_lTask;
};


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_DT_SCHEDULER_MJM012402_HPP
