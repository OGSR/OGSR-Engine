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

#ifndef BOOST_OT_CONTEXT_MJM012402_HPP
#define BOOST_OT_CONTEXT_MJM012402_HPP


#include <OpenTransport.h>

#include <boost/utility.hpp>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// class ot_context is intended to be used only as a singleton.  All that this class
//    does is ask OpenTransport to create him an OTClientContextPtr, and then doles
//    this out to anyone who wants it.  ot_context should only be instantiated at
//    system task time.

class ot_context: private noncopyable
{
  protected:
    ot_context();
    ~ot_context();

  public:
    OTClientContextPtr get_context();

  private:
    OTClientContextPtr m_pContext;
};


inline OTClientContextPtr ot_context::get_context()
    {    return(m_pContext);    }


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_OT_CONTEXT_MJM012402_HPP
