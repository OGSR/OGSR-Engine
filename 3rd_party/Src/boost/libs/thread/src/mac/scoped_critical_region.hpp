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

#ifndef BOOST_SCOPED_CRITICAL_REGION_MJM012402_HPP
#define BOOST_SCOPED_CRITICAL_REGION_MJM012402_HPP


#include <boost/thread/exceptions.hpp>

#include <Multiprocessing.h>


namespace boost {

namespace threads {

namespace mac {

namespace detail {


// class scoped_critical_region probably needs a new name.  Although the current name
//    is accurate, it can be read to mean that a critical region is entered for the
//    current scope.  In reality, a critical region is _created_ for the current scope.
//    This class is intended as a replacement for MPCriticalRegionID that will
//    automatically create and dispose of itself.

class scoped_critical_region
{
  public:
    scoped_critical_region();
    ~scoped_critical_region();

  public:
    operator const MPCriticalRegionID &() const;
    const MPCriticalRegionID &get() const;

  private:
    MPCriticalRegionID m_pCriticalRegionID;
};


// these are inlined for speed.
inline scoped_critical_region::operator const MPCriticalRegionID &() const
    {    return(m_pCriticalRegionID);    }
inline const MPCriticalRegionID &scoped_critical_region::get() const
    {    return(m_pCriticalRegionID);    }


} // namespace detail

} // namespace mac

} // namespace threads

} // namespace boost


#endif // BOOST_SCOPED_CRITICAL_REGION_MJM012402_HPP
