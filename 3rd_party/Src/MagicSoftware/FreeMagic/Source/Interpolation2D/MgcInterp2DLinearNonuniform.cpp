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

#include "MgcInterp2DLinearNonuniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DLinearNonuniform::Interp2DLinearNonuniform (int iVertexQuantity,
    Vector2* akVertex, Real* afF)
    :
    Delaunay2D(iVertexQuantity,akVertex)
{
    assert( afF );
    m_afF = afF;
}
//----------------------------------------------------------------------------
Interp2DLinearNonuniform::Interp2DLinearNonuniform (Delaunay2D& rkNet,
    Real* afF)
    :
    Delaunay2D(rkNet)
{
    assert( afF );
    m_afF = afF;
}
//----------------------------------------------------------------------------
Interp2DLinearNonuniform::~Interp2DLinearNonuniform ()
{
    delete[] m_afF;
}
//----------------------------------------------------------------------------
bool Interp2DLinearNonuniform::Evaluate (const Vector2& rkPoint, Real& rfF)
{
    // determine which triangle contains the target point
    Vector2 kV0, kV1, kV2;
    int i;
    for (i = 0; i < m_iTriangleQuantity; i++)
    {
        Triangle& rkTri = m_akTriangle[i];
        kV0 = m_akVertex[rkTri.m_aiVertex[0]];
        kV1 = m_akVertex[rkTri.m_aiVertex[1]];
        kV2 = m_akVertex[rkTri.m_aiVertex[2]];
        if ( InTriangle(kV0,kV1,kV2,rkPoint) )
            break;
    }

    if ( i == m_iTriangleQuantity )
    {
        // point is outside interpolation region
        return false;
    }

    // the input point is in this triangle
    Triangle& rkTri = m_akTriangle[i];

    // compute barycentric coordinates with respect to subtriangle
    Real afBary[3];
    ComputeBarycenter(kV0,kV1,kV2,rkPoint,afBary);

    // compute barycentric combination of function values at vertices
    rfF = afBary[0]*m_afF[rkTri.m_aiVertex[0]] +
        afBary[1]*m_afF[rkTri.m_aiVertex[1]] +
        afBary[2]*m_afF[rkTri.m_aiVertex[2]];

    return true;
}
//----------------------------------------------------------------------------


