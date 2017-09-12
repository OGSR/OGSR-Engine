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
inline EllipseStandard2::EllipseStandard2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Real& EllipseStandard2::Extent (int i)
{
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline const Real& EllipseStandard2::Extent (int i) const
{
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline Real* EllipseStandard2::Extents ()
{
    return m_afExtent;
}
//----------------------------------------------------------------------------
inline const Real* EllipseStandard2::Extents () const
{
    return m_afExtent;
}
//----------------------------------------------------------------------------
inline Ellipse2::Ellipse2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector2& Ellipse2::Center ()
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline const Vector2& Ellipse2::Center () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline Matrix2& Ellipse2::A ()
{
    return m_kA;
}
//----------------------------------------------------------------------------
inline const Matrix2& Ellipse2::A () const
{
    return m_kA;
}
//----------------------------------------------------------------------------


