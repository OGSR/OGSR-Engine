// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

//----------------------------------------------------------------------------
inline Capsule::Capsule ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector3& Capsule::Origin ()
{
    return m_kSegment.Origin();
}
//----------------------------------------------------------------------------
inline const Vector3& Capsule::Origin () const
{
    return m_kSegment.Origin();
}
//----------------------------------------------------------------------------
inline Vector3& Capsule::Direction ()
{
    return m_kSegment.Direction();
}
//----------------------------------------------------------------------------
inline const Vector3& Capsule::Direction () const
{
    return m_kSegment.Direction();
}
//----------------------------------------------------------------------------
inline Real& Capsule::Radius ()
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline const Real& Capsule::Radius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline Segment3& Capsule::Segment ()
{
    return m_kSegment;
}
//----------------------------------------------------------------------------
inline const Segment3& Capsule::Segment () const
{
    return m_kSegment;
}
//----------------------------------------------------------------------------


