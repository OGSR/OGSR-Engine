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
inline bool Delaunay2D::IsOwner () const
{
    return m_bOwner;
}
//----------------------------------------------------------------------------
inline int Delaunay2D::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline const Vector2& Delaunay2D::GetVertex (int i) const
{
    assert( 0 <= i && i < m_iVertexQuantity );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
inline const Vector2* Delaunay2D::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetXRange () const
{
    return m_fXRange;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay2D::GetYRange () const
{
    return m_fYRange;
}
//----------------------------------------------------------------------------
inline int Delaunay2D::GetEdgeQuantity () const
{
    return m_iEdgeQuantity;
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Edge& Delaunay2D::GetEdge (int i) const
{
    assert( 0 <= i && i < m_iEdgeQuantity );
    return m_akEdge[i];
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Edge* Delaunay2D::GetEdges () const
{
    return m_akEdge;
}
//----------------------------------------------------------------------------
inline int Delaunay2D::GetTriangleQuantity () const
{
    return m_iTriangleQuantity;
}
//----------------------------------------------------------------------------
inline Delaunay2D::Triangle& Delaunay2D::GetTriangle (int i)
{
    assert( 0 <= i && i < m_iTriangleQuantity );
    return m_akTriangle[i];
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Triangle& Delaunay2D::GetTriangle (
    int i) const
{
    assert( 0 <= i && i < m_iTriangleQuantity );
    return m_akTriangle[i];
}
//----------------------------------------------------------------------------
inline Delaunay2D::Triangle* Delaunay2D::GetTriangles ()
{
    return m_akTriangle;
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Triangle* Delaunay2D::GetTriangles () const
{
    return m_akTriangle;
}
//----------------------------------------------------------------------------
inline int Delaunay2D::GetExtraTriangleQuantity () const
{
    return m_iExtraTriangleQuantity;
}
//----------------------------------------------------------------------------
inline Delaunay2D::Triangle& Delaunay2D::GetExtraTriangle (int i)
{
    assert( 0 <= i && i < m_iExtraTriangleQuantity );
    return m_akExtraTriangle[i];
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Triangle& Delaunay2D::GetExtraTriangle (int i) const
{
    assert( 0 <= i && i < m_iExtraTriangleQuantity );
    return m_akExtraTriangle[i];
}
//----------------------------------------------------------------------------
inline Delaunay2D::Triangle* Delaunay2D::GetExtraTriangles ()
{
    return m_akExtraTriangle;
}
//----------------------------------------------------------------------------
inline const Delaunay2D::Triangle* Delaunay2D::GetExtraTriangles () const
{
    return m_akExtraTriangle;
}
//----------------------------------------------------------------------------
inline Real& Delaunay2D::Epsilon ()
{
    return ms_fEpsilon;
}
//----------------------------------------------------------------------------
inline Real& Delaunay2D::Range ()
{
    return ms_fRange;
}
//----------------------------------------------------------------------------
inline int& Delaunay2D::TSize ()
{
    return ms_iTSize;
}
//----------------------------------------------------------------------------

