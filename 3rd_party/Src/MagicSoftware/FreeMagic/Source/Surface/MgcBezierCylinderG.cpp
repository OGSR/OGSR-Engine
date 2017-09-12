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

#include "MgcBezierCylinderG.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierCylinderG::BezierCylinderG (int iDegree, Vector3* akCtrlPoint)
    :
    BezierPatchG(iDegree,2*(iDegree+1),akCtrlPoint)
{
    m_iCylinderLevel = 0;
}
//----------------------------------------------------------------------------
void BezierCylinderG::Initialize (int iLevel, bool bWantNormals)
{
    // vertices V = (2^C+1)*(2^L+1)
    int iTwoPowL = (1 << iLevel);
    int iTwoPowLPlusOne = iTwoPowL + 1;
    int iTwoPowC = (1 << m_iCylinderLevel);
    int iTwoPowCPlusOne = iTwoPowC + 1;
    m_iVertexQuantity = iTwoPowCPlusOne*iTwoPowLPlusOne;
    delete[] m_akVertex;
    m_akVertex = new Vector3[m_iVertexQuantity];

    // normals
    delete[] m_akNormal;
    if ( bWantNormals )
        m_akNormal = new Vector3[m_iVertexQuantity];

    // triangles T = 2*2^L*2^C
    m_iTriangleQuantity = 2*iTwoPowC*iTwoPowL;
    delete[] m_aiConnect;
    m_aiConnect = new int[3*m_iTriangleQuantity];

    // generate connectivity
    int* piConnect = m_aiConnect;
    for (int iY = 0, iYStart = 0; iY < iTwoPowC; iY++)
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

