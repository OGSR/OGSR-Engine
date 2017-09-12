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
inline int TubeSurfaceG::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline Vector3* TubeSurfaceG::GetVertices ()
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline Vector3* TubeSurfaceG::GetNormals ()
{
    return m_akNormal;
}
//----------------------------------------------------------------------------
inline int TubeSurfaceG::GetTriangleQuantity () const
{
    return m_iTriangleQuantity;
}
//----------------------------------------------------------------------------
inline const int* TubeSurfaceG::GetConnectivity () const
{
    return m_aiConnect;
}
//----------------------------------------------------------------------------
inline MultipleCurve3*& TubeSurfaceG::Medial ()
{
    return m_pkMedial;
}
//----------------------------------------------------------------------------
inline const MultipleCurve3* TubeSurfaceG::Medial () const
{
    return m_pkMedial;
}
//----------------------------------------------------------------------------
inline TubeSurfaceG::RadialFunction& TubeSurfaceG::Radial ()
{
    return m_oRadial;
}
//----------------------------------------------------------------------------
inline TubeSurfaceG::RadialFunction TubeSurfaceG::Radial () const
{
    return m_oRadial;
}
//----------------------------------------------------------------------------
inline bool& TubeSurfaceG::Closed ()
{
    return m_bClosed;
}
//----------------------------------------------------------------------------
inline bool TubeSurfaceG::Closed () const
{
    return m_bClosed;
}
//----------------------------------------------------------------------------
inline int TubeSurfaceG::GetMedialSamples () const
{
    return m_iMedialSamples;
}
//----------------------------------------------------------------------------
inline int TubeSurfaceG::GetSliceSamples () const
{
    return m_iSliceSamples;
}
//----------------------------------------------------------------------------
inline bool TubeSurfaceG::GetSampleByArcLength () const
{
    return m_bSampleByArcLength;
}
//----------------------------------------------------------------------------
inline bool TubeSurfaceG::GetWantNormals () const
{
    return m_bWantNormals;
}
//----------------------------------------------------------------------------
inline const Vector3* TubeSurfaceG::GetUpVector () const
{
    return m_pkUpVector;
}
//----------------------------------------------------------------------------
inline int TubeSurfaceG::Index (int iS, int iM)
{
    return iS + (m_iSliceSamples+1)*iM;
}
//----------------------------------------------------------------------------

