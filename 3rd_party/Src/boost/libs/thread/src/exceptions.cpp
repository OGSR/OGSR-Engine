// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#include <boost/thread/exceptions.hpp>
#include <string>

namespace boost {

lock_error::lock_error()
    : std::logic_error("thread lock error")
{
}

thread_resource_error::thread_resource_error()
    : std::runtime_error("thread resource error")
{
}

} // namespace boost
