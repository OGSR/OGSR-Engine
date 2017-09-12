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

#ifndef BOOST_OS_MJM012402_HPP
#define BOOST_OS_MJM012402_HPP


namespace boost {

namespace threads {

namespace mac {

namespace os {


// functions to determine the OS environment.  With namespaces, you get a cute call:
//    mac::os::x

bool x();
long version();


} // namespace os

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_OS_MJM012402_HPP
