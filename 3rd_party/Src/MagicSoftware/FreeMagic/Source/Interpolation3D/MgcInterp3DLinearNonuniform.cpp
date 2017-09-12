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

#include "MgcInterp3DLinearNonuniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp3DLinearNonuniform::Interp3DLinearNonuniform (int iVertexQuantity,
    Vector3* akVertex, Real* afF)
    :
    Delaunay3D(iVertexQuantity,akVertex)
{
    assert( afF );
    m_afF = afF;
}
//----------------------------------------------------------------------------
Interp3DLinearNonuniform::Interp3DLinearNonuniform (Delaunay3D& rkNet,
    Real* afF)
    :
    Delaunay3D(rkNet)
{
    assert( afF );
    m_afF = afF;
}
//----------------------------------------------------------------------------
Interp3DLinearNonuniform::~Interp3DLinearNonuniform ()
{
    delete[] m_afF;
}
//----------------------------------------------------------------------------
bool Interp3DLinearNonuniform::Evaluate (const Vector3& rkPoint, Real& rfF)
{
    // Determine which triangle contains the target point.
    //
    // TO DO.  This is an easy-to-implement, but slow search.  Better is to
    // start with a single tetrahedron.  If the point is inside, evaluate and
    // return.  If the point is outside, the next tetrahedron to visit is one
    // adjacent to the first.  The adjacent one you select is the one that is
    // intersected by the line segment whose end points are the centroid of
    // the current tetrahedron and the target point.  You effectively follow
    // a linear path through the tetrahedra to get to the target point.
    Vector3 kV0, kV1, kV2, kV3;
    Tetrahedron* pkTetra;
    Real afNumer[4], fDenom;
    int i;
    for (i = 0; i < m_iTetrahedronQuantity; i++)
    {
        pkTetra = &m_akTetrahedron[i];
        kV0 = m_akVertex[pkTetra->m_aiVertex[0]];
        kV1 = m_akVertex[pkTetra->m_aiVertex[1]];
        kV2 = m_akVertex[pkTetra->m_aiVertex[2]];
        kV3 = m_akVertex[pkTetra->m_aiVertex[3]];
        ComputeBarycenter(kV0,kV1,kV2,kV3,rkPoint,afNumer,fDenom);
        if ( InTetrahedron(afNumer,fDenom) )
            break;
    }

    if ( i == m_iTetrahedronQuantity )
    {
        // point is outside interpolation region
        return false;
    }

    // compute barycentric combination of function values at vertices
    rfF = (afNumer[0]*m_afF[pkTetra->m_aiVertex[0]] +
        afNumer[1]*m_afF[pkTetra->m_aiVertex[1]] +
        afNumer[2]*m_afF[pkTetra->m_aiVertex[2]] +
        afNumer[3]*m_afF[pkTetra->m_aiVertex[3]])/fDenom;

    return true;
}
//----------------------------------------------------------------------------


