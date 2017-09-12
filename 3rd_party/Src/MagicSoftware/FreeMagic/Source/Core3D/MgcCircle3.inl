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
inline Circle3::Circle3 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector3& Circle3::U ()
{
    return m_kU;
}
//----------------------------------------------------------------------------
inline const Vector3& Circle3::U () const
{
    return m_kU;
}
//----------------------------------------------------------------------------
inline Vector3& Circle3::V ()
{
    return m_kV;
}
//----------------------------------------------------------------------------
inline const Vector3& Circle3::V () const
{
    return m_kV;
}
//----------------------------------------------------------------------------
inline Vector3& Circle3::N ()
{
    return m_kN;
}
//----------------------------------------------------------------------------
inline const Vector3& Circle3::N () const
{
    return m_kN;
}
//----------------------------------------------------------------------------
inline Vector3& Circle3::Center ()
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline const Vector3& Circle3::Center () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline Real& Circle3::Radius ()
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline const Real& Circle3::Radius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------


