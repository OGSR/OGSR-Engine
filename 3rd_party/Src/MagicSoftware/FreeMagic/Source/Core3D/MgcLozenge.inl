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
inline Lozenge::Lozenge ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector3& Lozenge::Origin ()
{
    return m_kRectangle.Origin();
}
//----------------------------------------------------------------------------
inline const Vector3& Lozenge::Origin () const
{
    return m_kRectangle.Origin();
}
//----------------------------------------------------------------------------
inline Vector3& Lozenge::Edge0 ()
{
    return m_kRectangle.Edge0();
}
//----------------------------------------------------------------------------
inline const Vector3& Lozenge::Edge0 () const
{
    return m_kRectangle.Edge0();
}
//----------------------------------------------------------------------------
inline Vector3& Lozenge::Edge1 ()
{
    return m_kRectangle.Edge1();
}
//----------------------------------------------------------------------------
inline const Vector3& Lozenge::Edge1 () const
{
    return m_kRectangle.Edge1();
}
//----------------------------------------------------------------------------
inline Real& Lozenge::Radius ()
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline const Real& Lozenge::Radius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline Rectangle3& Lozenge::Rectangle ()
{
    return m_kRectangle;
}
//----------------------------------------------------------------------------
inline const Rectangle3& Lozenge::Rectangle () const
{
    return m_kRectangle;
}
//----------------------------------------------------------------------------


