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
inline Vector3& Frustum::Origin ()
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline const Vector3& Frustum::Origin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline Vector3& Frustum::LVector ()
{
    return m_kLVector;
}
//----------------------------------------------------------------------------
inline const Vector3& Frustum::LVector () const
{
    return m_kLVector;
}
//----------------------------------------------------------------------------
inline Vector3& Frustum::UVector ()
{
    return m_kUVector;
}
//----------------------------------------------------------------------------
inline const Vector3& Frustum::UVector () const
{
    return m_kUVector;
}
//----------------------------------------------------------------------------
inline Vector3& Frustum::DVector ()
{
    return m_kDVector;
}
//----------------------------------------------------------------------------
inline const Vector3& Frustum::DVector () const
{
    return m_kDVector;
}
//----------------------------------------------------------------------------
inline Real& Frustum::LBound ()
{
    return m_fLBound;
}
//----------------------------------------------------------------------------
inline const Real& Frustum::LBound () const
{
    return m_fLBound;
}
//----------------------------------------------------------------------------
inline Real& Frustum::UBound ()
{
    return m_fUBound;
}
//----------------------------------------------------------------------------
inline const Real& Frustum::UBound () const
{
    return m_fUBound;
}
//----------------------------------------------------------------------------
inline Real& Frustum::DMin ()
{
    return m_fDMin;
}
//----------------------------------------------------------------------------
inline const Real& Frustum::DMin () const
{
    return m_fDMin;
}
//----------------------------------------------------------------------------
inline Real& Frustum::DMax ()
{
    return m_fDMax;
}
//----------------------------------------------------------------------------
inline const Real& Frustum::DMax () const
{
    return m_fDMax;
}
//----------------------------------------------------------------------------
inline Real Frustum::GetDRatio () const
{
    return m_fDRatio;
}
//----------------------------------------------------------------------------
inline Real Frustum::GetMTwoLF () const
{
    return -2.0f*m_fLBound*m_fDMax;
}
//----------------------------------------------------------------------------
inline Real Frustum::GetMTwoUF () const
{
    return -2.0f*m_fUBound*m_fDMax;
}
//----------------------------------------------------------------------------


