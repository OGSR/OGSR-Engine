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

#include "MgcBezierTriangle2G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierTriangle2G::BezierTriangle2G (Vector3* akCtrlPoint)
    :
    BezierTriangleG(2,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetPosition (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;
    Real fU2 = fU*fU, fV2 = fV*fV, fW2 = fW*fW;
    Real f2UV = 2.0f*fU*fV, f2UW = 2.0f*fU*fW, f2VW = 2.0f*fV*fW;

    return fW2*m_akCtrlPoint[0] + f2UW*m_akCtrlPoint[1] +
        fU2*m_akCtrlPoint[2] + f2VW*m_akCtrlPoint[3] + f2UV*m_akCtrlPoint[4] +
        fV2*m_akCtrlPoint[5];
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetDerivativeU (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;

    return 2.0f*(-fW*m_akCtrlPoint[0] + (fW-fU)*m_akCtrlPoint[1] +
        fU*m_akCtrlPoint[2] - fV*m_akCtrlPoint[3] + fV*m_akCtrlPoint[4]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetDerivativeV (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;

    return 2.0f*(-fW*m_akCtrlPoint[0] - fU*m_akCtrlPoint[1] +
        (fW-fV)*m_akCtrlPoint[3] + fU*m_akCtrlPoint[4] +
        fV*m_akCtrlPoint[5]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetDerivativeUU (Real, Real) const
{
    return 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] + m_akCtrlPoint[2]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetDerivativeUV (Real, Real) const
{
    return 2.0f*(m_akCtrlPoint[0] - m_akCtrlPoint[1] - m_akCtrlPoint[3] +
        m_akCtrlPoint[4]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle2G::GetDerivativeVV (Real, Real) const
{
    return 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[3] + m_akCtrlPoint[5]);
}
//----------------------------------------------------------------------------
void BezierTriangle2G::Tessellate (int iLevel, bool bWantNormals)
{
    // allocate arrays and compute connectivity
    Initialize(iLevel,bWantNormals);

    // indices of three corners of patch, I0 (w=1), I1 (u=1), I2 (v=1)
    int iTwoPowL = (1 << iLevel);
    int i0 = 0;
    int i1 = iTwoPowL;
    int i2 = iTwoPowL*(iTwoPowL + 3) >> 1;

    // vertices for subdivision
    Vector3* akX = m_akVertex;
    akX[i0] = m_akCtrlPoint[0];
    akX[i1] = m_akCtrlPoint[2];
    akX[i2] = m_akCtrlPoint[5];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[i0] = 2.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[i1] = 2.0f*(m_akCtrlPoint[2] - m_akCtrlPoint[1]);
        akXu[i2] = 2.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[3]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[i0] = 2.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[0]);
        akXv[i1] = 2.0f*(m_akCtrlPoint[5] - m_akCtrlPoint[3]);
        akXv[i2] = 2.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[1]);
    }
    else
    {
        akXu = NULL;
        akXv = NULL;
    }

    // recursive subdivision
    if ( iLevel > 0 )
    {
        BlockParameters kBP;
        kBP.m_kXuu = 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] +
            m_akCtrlPoint[2]);
        kBP.m_kXvv = 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[3] +
            m_akCtrlPoint[5]);
        kBP.m_kXhh = 2.0f*(m_akCtrlPoint[0] + m_akCtrlPoint[4] -
            m_akCtrlPoint[1] - m_akCtrlPoint[3]);

        SubdivideLL(--iLevel,0.25f,akX,akXu,akXv,i0,i1,i2,kBP);
    }

    // calculate unit-length normals from derivative vectors
    if ( bWantNormals )
    {
        for (int i = 0; i < m_iVertexQuantity; i++)
            m_akNormal[i] = akXu[i].UnitCross(akXv[i]);
        delete[] akXu;
        delete[] akXv;
    }
}
//----------------------------------------------------------------------------
void BezierTriangle2G::SubdivideLL (int iLevel, Real fDSqr, Vector3* akX,
    Vector3* akXu, Vector3* akXv, int i0, int i1, int i2,
    BlockParameters& rkBP)
{
    /*
     i2
     +
     | \
     |   \
     +----+
     i0    i1
    */

    // subdivision indices
    int i01 = (i0 + i1) >> 1;
    int iD10 = i1 - i0;
    int i02 = (((i0 + i2) << 2) + iD10*iD10) >> 3;
    int i12 = i02 + (iD10 >> 1);

    // vertices

    // bottom u-edge subdivision
    akX[i01] = 0.5f*(akX[i0] + akX[i1] - fDSqr*rkBP.m_kXuu);

    // left v-edge subdivision
    akX[i02] = 0.5f*(akX[i0] + akX[i2] - fDSqr*rkBP.m_kXvv);

    // hypotenuse edge subdivision
    akX[i12] = 0.5f*(akX[i1] + akX[i2] - fDSqr*rkBP.m_kXhh);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        // bottom u-edge subdivision
        akXu[i01] = 0.5f*(akXu[i0] + akXu[i1]);
        akXv[i01] = 0.5f*(akXv[i0] + akXv[i1]);

        // left v-edge subdivision
        akXu[i02] = 0.5f*(akXu[i0] + akXu[i2]);
        akXv[i02] = 0.5f*(akXv[i0] + akXv[i2]);

        // hypotenuse edge subdivision
        akXu[i12] = 0.5f*(akXu[i1] + akXu[i2]);
        akXv[i12] = 0.5f*(akXv[i1] + akXv[i2]);
    }

    // recurse on four children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        // subtriangle <(s0,t0),(sm,t0),(s0,tm)>
        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i0,i01,i02,rkBP);

        // subtriangle <(sm,t0),(s1,t0),(sm,tm)>
        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i01,i1,i12,rkBP);

        // subtriangle <(s0,tm),(sm,tm),(s0,t1)>
        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i02,i12,i2,rkBP);

        // subtriangle <(sm,t0),(sm,tm),(s0,tm)>
        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i01,i12,i02,rkBP);
    }
}
//----------------------------------------------------------------------------
void BezierTriangle2G::SubdivideUR (int iLevel, Real fDSqr, Vector3* akX,
    Vector3* akXu, Vector3* akXv, int i0, int i1, int i2,
    BlockParameters& rkBP)
{
    /*
     i2   i1
     +----+
       \  |
         \|
          +
          i0
    */

    // subdivision indices
    int i12 = (i1 + i2) >> 1;
    int iD12 = i1 - i2;
    int i01 = (((i0 + i1) << 2) + iD12*iD12) >> 3;
    int i02 = i01 - (iD12 >> 1);

    // vertices

    // top u-edge subdivision
    akX[i12] = 0.5f*(akX[i1] + akX[i2] - fDSqr*rkBP.m_kXuu);

    // right v-edge subdivision
    akX[i01] = 0.5f*(akX[i0] + akX[i1] - fDSqr*rkBP.m_kXvv);

    // hypotenuse edge subdivision
    akX[i02] = 0.5f*(akX[i0] + akX[i2] - fDSqr*rkBP.m_kXhh);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        // top u-edge subdivision
        akXu[i12] = 0.5f*(akXu[i1] + akXu[i2]);
        akXv[i12] = 0.5f*(akXv[i1] + akXv[i2]);

        // right v-edge subdivision
        akXu[i01] = 0.5f*(akXu[i0] + akXu[i1]);
        akXv[i01] = 0.5f*(akXv[i0] + akXv[i1]);

        // hypotenuse edge subdivision
        akXu[i02] = 0.5f*(akXu[i0] + akXu[i2]);
        akXv[i02] = 0.5f*(akXv[i0] + akXv[i2]);
    }

    // recurse on four children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        // subtriangle <(sm,tm),(sm,t1),(s0,t1)>
        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i02,i12,i2,rkBP);

        // subtriangle <(s1,tm),(s1,t1),(sm,t1)>
        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i01,i1,i12,rkBP);

        // subtriangle <(s1,t0),(s1,tm),(sm,tm)>
        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i0,i01,i02,rkBP);

        // subtriangle <(sm,tm),(s1,tm),(sm,t1)>
        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i02,i01,i12,rkBP);
    }
}
//----------------------------------------------------------------------------

