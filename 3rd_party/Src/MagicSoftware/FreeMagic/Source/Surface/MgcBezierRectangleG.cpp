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

#include "MgcBezierRectangleG.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierRectangleG::BezierRectangleG (int iDegree, Vector3* akCtrlPoint)
    :
    BezierPatchG(iDegree,(iDegree+1)*(iDegree+1),akCtrlPoint)
{
}
//----------------------------------------------------------------------------
void BezierRectangleG::Initialize (int iLevel, bool bWantNormals)
{
    // vertices V = (2^L+1)^2
    int iTwoPowL = (1 << iLevel);
    int iTwoPowLPlusOne = iTwoPowL + 1;
    m_iVertexQuantity = iTwoPowLPlusOne*iTwoPowLPlusOne;
    delete[] m_akVertex;
    m_akVertex = new Vector3[m_iVertexQuantity];

    // normals
    delete[] m_akNormal;
    if ( bWantNormals )
        m_akNormal = new Vector3[m_iVertexQuantity];

    // triangles T = 2*4^L
    m_iTriangleQuantity = 2*(1 << 2*iLevel);
    delete[] m_aiConnect;
    m_aiConnect = new int[3*m_iTriangleQuantity];

    // generate connectivity
    int* piConnect = m_aiConnect;
    for (int iY = 0, iYStart = 0; iY < iTwoPowL; iY++)
    {
        int iI0 = iYStart;
        iYStart += iTwoPowLPlusOne;
        int iI1 = iI0 + 1;
        int iI2 = iYStart;
        int iI3 = iI2 + 1;
        for (int iX = 0; iX < iTwoPowL; iX++)
        {
            *piConnect++ = iI0;
            *piConnect++ = iI1;
            *piConnect++ = iI2;
            *piConnect++ = iI1;
            *piConnect++ = iI3;
            *piConnect++ = iI2;
            iI0++;
            iI1++;
            iI2++;
            iI3++;
        }
    }
}
//----------------------------------------------------------------------------

