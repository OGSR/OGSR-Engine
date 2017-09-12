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
inline bool Delaunay3D::IsOwner () const
{
    return m_bOwner;
}
//----------------------------------------------------------------------------
inline int Delaunay3D::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline const Vector3& Delaunay3D::GetVertex (int i) const
{
    assert( 0 <= i && i < m_iVertexQuantity );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
inline const Vector3* Delaunay3D::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXRange () const
{
    return m_fXRange;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYRange () const
{
    return m_fYRange;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZMin () const
{
    return m_fZMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZMax () const
{
    return m_fZMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZRange () const
{
    return m_fZRange;
}
//----------------------------------------------------------------------------
inline int Delaunay3D::GetTetrahedronQuantity () const
{
    return m_iTetrahedronQuantity;
}
//----------------------------------------------------------------------------
inline Delaunay3D::Tetrahedron& Delaunay3D::GetTetrahedron (int i)
{
    assert( 0 <= i && i < m_iTetrahedronQuantity );
    return m_akTetrahedron[i];
}
//----------------------------------------------------------------------------
inline const Delaunay3D::Tetrahedron& Delaunay3D::GetTetrahedron (int i) const
{
    assert( 0 <= i && i < m_iTetrahedronQuantity );
    return m_akTetrahedron[i];
}
//----------------------------------------------------------------------------
inline Delaunay3D::Tetrahedron* Delaunay3D::GetTetrahedrons ()
{
    return m_akTetrahedron;
}
//----------------------------------------------------------------------------
inline const Delaunay3D::Tetrahedron* Delaunay3D::GetTetrahedrons () const
{
    return m_akTetrahedron;
}
//----------------------------------------------------------------------------
inline Real& Delaunay3D::Epsilon ()
{
    return ms_fEpsilon;
}
//----------------------------------------------------------------------------
inline Real& Delaunay3D::Range ()
{
    return ms_fRange;
}
//----------------------------------------------------------------------------
inline int& Delaunay3D::TSize ()
{
    return ms_iTSize;
}
//----------------------------------------------------------------------------
inline int& Delaunay3D::QuantityFactor ()
{
    return ms_iQuantityFactor;
}
//----------------------------------------------------------------------------

