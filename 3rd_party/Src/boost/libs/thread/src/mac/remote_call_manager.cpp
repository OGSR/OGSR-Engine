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

#include "remote_call_manager.hpp"

#include <boost/bind.hpp>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


using detail::delivery_man;


remote_call_manager::remote_call_manager():
    m_oDTDeliveryMan(),
    m_oSTDeliveryMan(),
    m_oDTFunction(bind(&delivery_man::accept_deliveries, &m_oDTDeliveryMan)),
    m_oSTFunction(bind(&delivery_man::accept_deliveries, &m_oSTDeliveryMan)),
    m_oDTPeriodical(m_oDTFunction),
    m_oSTPeriodical(m_oSTFunction)
{
    m_oDTPeriodical.start();
    m_oSTPeriodical.start();
}

remote_call_manager::~remote_call_manager()
{
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
