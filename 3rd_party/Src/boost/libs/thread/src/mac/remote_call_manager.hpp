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

#ifndef BOOST_REMOTE_CALL_MANAGER_MJM012402_HPP
#define BOOST_REMOTE_CALL_MANAGER_MJM012402_HPP


#include <boost/utility.hpp>

#include "delivery_man.hpp"
#include "dt_scheduler.hpp"
#include "periodical.hpp"
#include "execution_context.hpp"
#include "st_scheduler.hpp"


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// class remote_call_manager is used by the remote call functions (dt_remote_call and
//  st_remote_call) to execute functions in non-MP contexts.

class remote_call_manager: private noncopyable
{
  protected:
    remote_call_manager();
    ~remote_call_manager();

  public:
    template<class R>
    R execute_at_dt(function<R> &rFunctor);
    template<class R>
    R execute_at_st(function<R> &rFunctor);

  private:
    template<class R>
    static R execute_now(function<R> &rFunctor);

  private:
    delivery_man m_oDTDeliveryMan;
    delivery_man m_oSTDeliveryMan;
    function<void> m_oDTFunction;
    function<void> m_oSTFunction;
    periodical<dt_scheduler> m_oDTPeriodical;
    periodical<st_scheduler> m_oSTPeriodical;
};


template<class R>
/*static*/ inline R remote_call_manager::execute_now(function<R> &rFunctor)
{
    return(rFunctor());
}
template<>
/*static*/ inline void remote_call_manager::execute_now<void>(function<void> &rFunctor)
{
    rFunctor();
}


template<class R>
inline R remote_call_manager::execute_at_dt(function<R> &rFunctor)
{
    if(at_mp())
    {
        return(m_oDTDeliveryMan.deliver(rFunctor));
    }
    return(execute_now(rFunctor));
}

template<class R>
inline R remote_call_manager::execute_at_st(function<R> &rFunctor)
{
    if(at_mp())
    {
        return(m_oSTDeliveryMan.deliver(rFunctor));
    }
    assert(at_st());
    return(execute_now(rFunctor));
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_REMOTE_CALL_MANAGER_MJM012402_HPP
