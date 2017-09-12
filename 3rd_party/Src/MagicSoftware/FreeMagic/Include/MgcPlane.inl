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
inline Plane::Plane ()
{
    m_kNormal = Vector3::ZERO;
    m_fConstant = 0.0f;
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Vector3& rkNormal, Real fConstant)
{
    m_kNormal = rkNormal;
    m_fConstant = fConstant;
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Vector3& rkNormal, const Vector3& rkPoint)
{
    m_kNormal = rkNormal;
    m_fConstant = rkNormal.Dot(rkPoint);
}
//----------------------------------------------------------------------------
inline Real& Plane::operator[] (int i) const
{
    return ((Real*)this)[i];
}
//----------------------------------------------------------------------------
inline Plane::operator Real* ()
{
    return (Real*)this;
}
//----------------------------------------------------------------------------
inline Vector3& Plane::Normal ()
{
    return m_kNormal;
}
//----------------------------------------------------------------------------
inline const Vector3& Plane::Normal () const
{
    return m_kNormal;
}
//----------------------------------------------------------------------------
inline Real& Plane::Constant ()
{
    return m_fConstant;
}
//----------------------------------------------------------------------------
inline const Real& Plane::Constant () const
{
    return m_fConstant;
}
//----------------------------------------------------------------------------
inline Real Plane::DistanceTo (const Vector3& rkPoint) const
{
    return m_kNormal.Dot(rkPoint) - m_fConstant;
}
//----------------------------------------------------------------------------


