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
inline Cone3::Cone3 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector3& Cone3::Vertex ()
{
    return m_kVertex;
}
//----------------------------------------------------------------------------
inline const Vector3& Cone3::Vertex () const
{
    return m_kVertex;
}
//----------------------------------------------------------------------------
inline Vector3& Cone3::Axis ()
{
    return m_kAxis;
}
//----------------------------------------------------------------------------
inline const Vector3& Cone3::Axis () const
{
    return m_kAxis;
}
//----------------------------------------------------------------------------
inline Real& Cone3::CosAngle ()
{
    return m_fCos;
}
//----------------------------------------------------------------------------
inline const Real& Cone3::CosAngle () const
{
    return m_fCos;
}
//----------------------------------------------------------------------------
inline Real& Cone3::SinAngle ()
{
    return m_fSin;
}
//----------------------------------------------------------------------------
inline const Real& Cone3::SinAngle () const
{
    return m_fSin;
}
//----------------------------------------------------------------------------


