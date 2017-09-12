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

#ifndef BOOST_PERIODICAL_MJM012402_HPP
#define BOOST_PERIODICAL_MJM012402_HPP


#include <boost/function.hpp>
#include <boost/utility.hpp>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// class periodical inherits from its template parameter, which should follow the
//    pattern set by classes dt_scheduler and st_scheduler.  periodical knows how to
//    call a boost::function, where the xx_scheduler classes only know to to call a
//    member periodically.

template<class Scheduler>
class periodical: private noncopyable, private Scheduler
{
  public:
    periodical(function<void> &rFunction);
    ~periodical();

  public:
    void start();
    void stop();

  protected:
    virtual void periodic_function();

  private:
    function<void> m_oFunction;
};


template<class Scheduler>
periodical<Scheduler>::periodical(function<void> &rFunction):
    m_oFunction(rFunction)
{
// no-op
}

template<class Scheduler>
periodical<Scheduler>::~periodical()
{
    stop();
}

template<class Scheduler>
void periodical<Scheduler>::start()
{
    start_polling();
}

template<class Scheduler>
void periodical<Scheduler>::stop()
{
    stop_polling();
}


template<class Scheduler>
inline void periodical<Scheduler>::periodic_function()
{
    try
    {
        m_oFunction();
    }
    catch(...)
    {
    }
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_PERIODICAL_MJM012402_HPP
