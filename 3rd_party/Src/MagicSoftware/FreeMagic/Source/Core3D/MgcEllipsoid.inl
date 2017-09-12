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
inline EllipsoidStandard::EllipsoidStandard ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Real& EllipsoidStandard::Extent (int i)
{
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline const Real& EllipsoidStandard::Extent (int i) const
{
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline Real* EllipsoidStandard::Extents ()
{
    return m_afExtent;
}
//----------------------------------------------------------------------------
inline const Real* EllipsoidStandard::Extents () const
{
    return m_afExtent;
}
//----------------------------------------------------------------------------
inline Ellipsoid::Ellipsoid ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector3& Ellipsoid::Center ()
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline const Vector3& Ellipsoid::Center () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline Matrix3& Ellipsoid::A ()
{
    return m_kA;
}
//----------------------------------------------------------------------------
inline const Matrix3& Ellipsoid::A () const
{
    return m_kA;
}
//----------------------------------------------------------------------------
inline Matrix3& Ellipsoid::InverseA ()
{
    return m_kInverseA;
}
//----------------------------------------------------------------------------
inline const Matrix3& Ellipsoid::InverseA () const
{
    return m_kInverseA;
}
//----------------------------------------------------------------------------


