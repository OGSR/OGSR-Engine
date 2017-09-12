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

#include "ot_context.hpp"

#include "execution_context.hpp"


#include <cassert>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


ot_context::ot_context()
{
    assert(at_st());

    OSStatus lStatus = InitOpenTransportInContext(0UL, &m_pContext);
// TODO - throw on error
    assert(lStatus == noErr);
}

ot_context::~ot_context()
{
    CloseOpenTransportInContext(m_pContext);
}


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost
