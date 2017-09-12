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

#include "delivery_man.hpp"

#include "os.hpp"
#include "execution_context.hpp"


namespace boost {

namespace threads {

namespace mac {

namespace detail {


delivery_man::delivery_man():
    m_pPackage(NULL),
    m_pSemaphore(kInvalidID),
    m_bPackageWaiting(false)
{
    assert(at_st());

    OSStatus lStatus = MPCreateSemaphore(1UL, 0UL, &m_pSemaphore);
// TODO - throw on error here
    assert(lStatus == noErr);
}

delivery_man::~delivery_man()
{
    assert(m_bPackageWaiting == false);

    OSStatus lStatus = MPDeleteSemaphore(m_pSemaphore);
    assert(lStatus == noErr);
}


void delivery_man::accept_deliveries()
{
    if(m_bPackageWaiting)
    {
        assert(m_pPackage != NULL);
        m_pPackage->accept();
        m_pPackage = NULL;
        m_bPackageWaiting = false;

    // signal to the thread making the call that we're done
        OSStatus lStatus = MPSignalSemaphore(m_pSemaphore);
        assert(lStatus == noErr);
    }
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
