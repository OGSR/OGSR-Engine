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

#include "MgcBezierTriangle3G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierTriangle3G::BezierTriangle3G (Vector3* akCtrlPoint)
    :
    BezierTriangleG(3,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetPosition (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;
    Real fU2 = fU*fU, fV2 = fV*fV, fW2 = fW*fW;
    Real fU3 = fU*fU2, fV3 = fV*fV2, fW3 = fW*fW2;
    Real f3UV2 = 3.0f*fU*fV2, f3UW2 = 3.0f*fU*fW2, f3VW2 = 3.0f*fV*fW2;
    Real f3U2V = 3.0f*fU2*fV, f3U2W = 3.0f*fU2*fW, f3V2W = 3.0f*fV2*fW;
    Real f6UVW = 6.0f*fU*fV*fW;

    return fW3*m_akCtrlPoint[0] + f3UW2*m_akCtrlPoint[1] +
        f3U2W*m_akCtrlPoint[2] + fU3*m_akCtrlPoint[3] +
        f3VW2*m_akCtrlPoint[4] + f6UVW*m_akCtrlPoint[5] +
        f3U2V*m_akCtrlPoint[6] + f3V2W*m_akCtrlPoint[7] +
        f3UV2*m_akCtrlPoint[8] + fV3*m_akCtrlPoint[9];
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetDerivativeU (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;
    Real f3U2 = 3.0f*fU*fU, f3V2 = 3.0f*fV*fV, f3W2 = 3.0f*fW*fW;
    Real f6UV = 6.0f*fU*fV, f6UW = 6.0f*fU*fW, f6VW = 6.0f*fV*fW;

    return -f3W2*m_akCtrlPoint[0] + (f3W2-f6UW)*m_akCtrlPoint[1] +
        (f6UW-f3U2)*m_akCtrlPoint[2] + f3U2*m_akCtrlPoint[3] -
        f6VW*m_akCtrlPoint[4] + (f6VW-f6UV)*m_akCtrlPoint[5] +
        f6UV*m_akCtrlPoint[6] - f3V2*m_akCtrlPoint[7] +
        f3V2*m_akCtrlPoint[8];
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetDerivativeV (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;
    Real f3U2 = 3.0f*fU*fU, f3V2 = 3.0f*fV*fV, f3W2 = 3.0f*fW*fW;
    Real f6UV = 6.0f*fU*fV, f6UW = 6.0f*fU*fW, f6VW = 6.0f*fV*fW;

    return -f3W2*m_akCtrlPoint[0] - f6UW*m_akCtrlPoint[1] -
        f3U2*m_akCtrlPoint[2] + (f3W2-f6VW)*m_akCtrlPoint[4] +
        (f6UW-f6UV)*m_akCtrlPoint[5] + f3U2*m_akCtrlPoint[6] +
        (f6VW-f3V2)*m_akCtrlPoint[7] + f6UV*m_akCtrlPoint[8] +
        f3V2*m_akCtrlPoint[9];
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetDerivativeUU (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;

    return 6.0f*(fW*m_akCtrlPoint[0] + (fU-2.0f*fW)*m_akCtrlPoint[1] +
        (fW-2.0f*fU)*m_akCtrlPoint[2] + fU*m_akCtrlPoint[3] +
        fV*m_akCtrlPoint[4] - 2.0f*m_akCtrlPoint[5] + fV*m_akCtrlPoint[6]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetDerivativeUV (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;

    return 6.0f*(fW*m_akCtrlPoint[0] + (fU-fW)*m_akCtrlPoint[1] -
        fU*m_akCtrlPoint[2] + (fV-fW)*m_akCtrlPoint[4] +
        (fW-fU-fV)*m_akCtrlPoint[5] + fU*m_akCtrlPoint[6] -
        fV*m_akCtrlPoint[7] + fV*m_akCtrlPoint[8]);
}
//----------------------------------------------------------------------------
Vector3 BezierTriangle3G::GetDerivativeVV (Real fU, Real fV) const
{
    Real fW = 1.0f - fU - fV;

    return 6.0f*(fW*m_akCtrlPoint[0] + fU*m_akCtrlPoint[1] +
        (fV-2.0f*fW)*m_akCtrlPoint[4] - 2.0f*fU*m_akCtrlPoint[5] +
        (fW-2.0f*fV)*m_akCtrlPoint[7] + fU*m_akCtrlPoint[8] +
        fV*m_akCtrlPoint[9]);
}
//----------------------------------------------------------------------------
void BezierTriangle3G::Tessellate (int iLevel, bool bWantNormals)
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
    akX[i1] = m_akCtrlPoint[3];
    akX[i2] = m_akCtrlPoint[9];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[i0] = 3.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[i1] = 3.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[2]);
        akXu[i2] = 3.0f*(m_akCtrlPoint[8] - m_akCtrlPoint[7]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[i0] = 3.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[0]);
        akXv[i1] = 3.0f*(m_akCtrlPoint[6] - m_akCtrlPoint[2]);
        akXv[i2] = 3.0f*(m_akCtrlPoint[9] - m_akCtrlPoint[7]);
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
        kBP.m_aakXuu[0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] +
            m_akCtrlPoint[2]);
        kBP.m_aakXuu[1] = 6.0f*(m_akCtrlPoint[1] - 2.0f*m_akCtrlPoint[2] +
            m_akCtrlPoint[3]);
        kBP.m_aakXuu[2] = 6.0f*(m_akCtrlPoint[4] - 2.0f*m_akCtrlPoint[5] +
            m_akCtrlPoint[6]);
        kBP.m_aakXvv[0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[4] +
            m_akCtrlPoint[7]);
        kBP.m_aakXvv[1] = 6.0f*(m_akCtrlPoint[1] - 2.0f*m_akCtrlPoint[5] +
            m_akCtrlPoint[8]);
        kBP.m_aakXvv[2] = 6.0f*(m_akCtrlPoint[4] - 2.0f*m_akCtrlPoint[7] +
            m_akCtrlPoint[9]);
        kBP.m_aakXhh[0] = 6.0f*(m_akCtrlPoint[2] - 2.0f*m_akCtrlPoint[5] +
            m_akCtrlPoint[7]);
        kBP.m_aakXhh[1] = 6.0f*(m_akCtrlPoint[3] - 2.0f*m_akCtrlPoint[6] +
            m_akCtrlPoint[8]);
        kBP.m_aakXhh[2] = 6.0f*(m_akCtrlPoint[6] - 2.0f*m_akCtrlPoint[8] +
            m_akCtrlPoint[9]);

        if ( bWantNormals )
        {
            kBP.m_kXuuu = 6.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[0] +
                2.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[2]));
            kBP.m_kXuuv = 6.0f*(m_akCtrlPoint[4] + m_akCtrlPoint[6] -
                m_akCtrlPoint[0] - m_akCtrlPoint[2] + 2.0f*(m_akCtrlPoint[1] -
                m_akCtrlPoint[5]));
            kBP.m_kXuvv = 6.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[8] -
                m_akCtrlPoint[0] - m_akCtrlPoint[7] + 2.0f*(m_akCtrlPoint[4] -
                m_akCtrlPoint[5]));
            kBP.m_kXvvv = 6.0f*(m_akCtrlPoint[9] - m_akCtrlPoint[0] +
                2.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[7]));
            kBP.m_kXhhu = 6.0f*(m_akCtrlPoint[3] + m_akCtrlPoint[8] -
                m_akCtrlPoint[2] - m_akCtrlPoint[7] + 2.0f*(m_akCtrlPoint[5] -
                m_akCtrlPoint[6]));
            kBP.m_kXhhv = 6.0f*(m_akCtrlPoint[6] + m_akCtrlPoint[9] -
                m_akCtrlPoint[2] - m_akCtrlPoint[7] + 2.0f*(m_akCtrlPoint[5] -
                m_akCtrlPoint[8]));
        }

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
void BezierTriangle3G::SubdivideLL (int iLevel, Real fDSqr, Vector3* akX,
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
    int i02 = (((i0+i2) << 2) + iD10*iD10) >> 3;
    int i12 = i02 + (iD10 >> 1);

    // vertices

    // bottom u-edge subdivision
    Vector3 kXuu01 = 0.5f*(rkBP.m_aakXuu[0] + rkBP.m_aakXuu[1]);
    Vector3 kXvv01 = 0.5f*(rkBP.m_aakXvv[0] + rkBP.m_aakXvv[1]);
    Vector3 kXhh01 = 0.5f*(rkBP.m_aakXhh[0] + rkBP.m_aakXhh[1]);
    akX[i01] = 0.5f*(akX[i0] + akX[i1] - fDSqr*kXuu01);

    // left v-edge subdivision
    Vector3 kXuu02 = 0.5f*(rkBP.m_aakXuu[0] + rkBP.m_aakXuu[2]);
    Vector3 kXvv02 = 0.5f*(rkBP.m_aakXvv[0] + rkBP.m_aakXvv[2]);
    Vector3 kXhh02 = 0.5f*(rkBP.m_aakXhh[0] + rkBP.m_aakXhh[2]);
    akX[i02] = 0.5f*(akX[i0] + akX[i2] - fDSqr*kXvv02);

    // hypotenuse edge subdivision
    Vector3 kXuu12 = 0.5f*(rkBP.m_aakXuu[1] + rkBP.m_aakXuu[2]);
    Vector3 kXvv12 = 0.5f*(rkBP.m_aakXvv[1] + rkBP.m_aakXvv[2]);
    Vector3 kXhh12 = 0.5f*(rkBP.m_aakXhh[1] + rkBP.m_aakXhh[2]);
    akX[i12] = 0.5f*(akX[i1] + akX[i2] - fDSqr*kXhh12);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        // bottom u-edge subdivision
        akXu[i01] = 0.5f*(akXu[i0] + akXu[i1] - fDSqr*rkBP.m_kXuuu);
        akXv[i01] = 0.5f*(akXv[i0] + akXv[i1] - fDSqr*rkBP.m_kXuuv);

        // left v-edge subdivision
        akXu[i02] = 0.5f*(akXu[i0] + akXu[i2] - fDSqr*rkBP.m_kXuvv);
        akXv[i02] = 0.5f*(akXv[i0] + akXv[i2] - fDSqr*rkBP.m_kXvvv);

        // hypotenuse edge subdivision
        akXu[i12] = 0.5f*(akXu[i1] + akXu[i2] - fDSqr*rkBP.m_kXhhu);
        akXv[i12] = 0.5f*(akXv[i1] + akXv[i2] - fDSqr*rkBP.m_kXhhv);
    }

    // recurse on four children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        BlockParameters kSubBP;

        // subtriangle <(s0,t0),(sm,t0),(s0,tm)>
        kSubBP.m_aakXuu[0] = rkBP.m_aakXuu[0];
        kSubBP.m_aakXuu[1] = kXuu01;
        kSubBP.m_aakXuu[2] = kXuu02;
        kSubBP.m_aakXvv[0] = rkBP.m_aakXvv[0];
        kSubBP.m_aakXvv[1] = kXvv01;
        kSubBP.m_aakXvv[2] = kXvv02;
        kSubBP.m_aakXhh[0] = rkBP.m_aakXhh[0];
        kSubBP.m_aakXhh[1] = kXhh01;
        kSubBP.m_aakXhh[2] = kXhh02;

        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i0,i01,i02,kSubBP);

        // subtriangle <(sm,t0),(s1,t0),(sm,tm)>
        kSubBP.m_aakXuu[0] = kXuu01;
        kSubBP.m_aakXuu[1] = rkBP.m_aakXuu[1];
        kSubBP.m_aakXuu[2] = kXuu12;
        kSubBP.m_aakXvv[0] = kXvv01;
        kSubBP.m_aakXvv[1] = rkBP.m_aakXvv[1];
        kSubBP.m_aakXvv[2] = kXvv12;
        kSubBP.m_aakXhh[0] = kXhh01;
        kSubBP.m_aakXhh[1] = rkBP.m_aakXhh[1];
        kSubBP.m_aakXhh[2] = kXhh12;

        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i01,i1,i12,kSubBP);

        // subtriangle <(s0,tm),(sm,tm),(s0,t1)>
        kSubBP.m_aakXuu[0] = kXuu02;
        kSubBP.m_aakXuu[1] = kXuu12;
        kSubBP.m_aakXuu[2] = rkBP.m_aakXuu[2];
        kSubBP.m_aakXvv[0] = kXvv02;
        kSubBP.m_aakXvv[1] = kXvv12;
        kSubBP.m_aakXvv[2] = rkBP.m_aakXvv[2];
        kSubBP.m_aakXhh[0] = kXhh02;
        kSubBP.m_aakXhh[1] = kXhh12;
        kSubBP.m_aakXhh[2] = rkBP.m_aakXhh[2];

        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i02,i12,i2,kSubBP);

        // subtriangle <(sm,t0),(sm,tm),(s0,tm)>
        kSubBP.m_aakXuu[0] = kXuu01;
        kSubBP.m_aakXuu[1] = kXuu12;
        kSubBP.m_aakXuu[2] = kXuu02;
        kSubBP.m_aakXvv[0] = kXvv01;
        kSubBP.m_aakXvv[1] = kXvv12;
        kSubBP.m_aakXvv[2] = kXvv02;
        kSubBP.m_aakXhh[0] = kXhh01;
        kSubBP.m_aakXhh[1] = kXhh12;
        kSubBP.m_aakXhh[2] = kXhh02;

        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i01,i12,i02,kSubBP);
    }
}
//----------------------------------------------------------------------------
void BezierTriangle3G::SubdivideUR (int iLevel, Real fDSqr, Vector3* akX,
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
    int i01 = (((i0+i1) << 2) + iD12*iD12) >> 3;
    int i02 = i01 - (iD12 >> 1);

    // vertices

    // top u-edge subdivision
    Vector3 kXuu12 = 0.5f*(rkBP.m_aakXuu[1] + rkBP.m_aakXuu[2]);
    Vector3 kXvv12 = 0.5f*(rkBP.m_aakXvv[1] + rkBP.m_aakXvv[2]);
    Vector3 kXhh12 = 0.5f*(rkBP.m_aakXhh[1] + rkBP.m_aakXhh[2]);
    akX[i12] = 0.5f*(akX[i1] + akX[i2] - fDSqr*kXuu12);

    // right v-edge subdivision
    Vector3 kXuu01 = 0.5f*(rkBP.m_aakXuu[0] + rkBP.m_aakXuu[1]);
    Vector3 kXvv01 = 0.5f*(rkBP.m_aakXvv[0] + rkBP.m_aakXvv[1]);
    Vector3 kXhh01 = 0.5f*(rkBP.m_aakXhh[0] + rkBP.m_aakXhh[1]);
    akX[i01] = 0.5f*(akX[i0] + akX[i1] - fDSqr*kXvv01);

    // hypotenuse edge subdivision
    Vector3 kXuu02 = 0.5f*(rkBP.m_aakXuu[0] + rkBP.m_aakXuu[2]);
    Vector3 kXvv02 = 0.5f*(rkBP.m_aakXvv[0] + rkBP.m_aakXvv[2]);
    Vector3 kXhh02 = 0.5f*(rkBP.m_aakXhh[0] + rkBP.m_aakXhh[2]);
    akX[i02] = 0.5f*(akX[i0] + akX[i2] - fDSqr*kXhh02);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        // top u-edge subdivision
        akXu[i12] = 0.5f*(akXu[i1] + akXu[i2] - fDSqr*rkBP.m_kXuuu);
        akXv[i12] = 0.5f*(akXv[i1] + akXv[i2] - fDSqr*rkBP.m_kXuuv);

        // right v-edge subdivision
        akXu[i01] = 0.5f*(akXu[i0] + akXu[i1] - fDSqr*rkBP.m_kXuvv);
        akXv[i01] = 0.5f*(akXv[i0] + akXv[i1] - fDSqr*rkBP.m_kXvvv);

        // hypotenuse edge subdivision
        akXu[i02] = 0.5f*(akXu[i0] + akXu[i2] - fDSqr*rkBP.m_kXhhu);
        akXv[i02] = 0.5f*(akXv[i0] + akXv[i2] - fDSqr*rkBP.m_kXhhv);
    }

    // recurse on four children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        BlockParameters kSubBP;

        // subtriangle <(sm,tm),(sm,t1),(s0,t1)>
        kSubBP.m_aakXuu[0] = kXuu02;
        kSubBP.m_aakXuu[1] = kXuu12;
        kSubBP.m_aakXuu[2] = rkBP.m_aakXuu[2];
        kSubBP.m_aakXvv[0] = kXvv02;
        kSubBP.m_aakXvv[1] = kXvv12;
        kSubBP.m_aakXvv[2] = rkBP.m_aakXvv[2];
        kSubBP.m_aakXhh[0] = kXhh02;
        kSubBP.m_aakXhh[1] = kXhh12;
        kSubBP.m_aakXhh[2] = rkBP.m_aakXhh[2];

        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i02,i12,i2,kSubBP);

        // subtriangle <(s1,tm),(s1,t1),(sm,t1)>
        kSubBP.m_aakXuu[0] = kXuu01;
        kSubBP.m_aakXuu[1] = rkBP.m_aakXuu[1];
        kSubBP.m_aakXuu[2] = kXuu12;
        kSubBP.m_aakXvv[0] = kXvv01;
        kSubBP.m_aakXvv[1] = rkBP.m_aakXvv[1];
        kSubBP.m_aakXvv[2] = kXvv12;
        kSubBP.m_aakXhh[0] = kXhh01;
        kSubBP.m_aakXhh[1] = rkBP.m_aakXhh[1];
        kSubBP.m_aakXhh[2] = kXhh12;

        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i01,i1,i12,kSubBP);

        // subtriangle <(s1,t0),(s1,tm),(sm,tm)>
        kSubBP.m_aakXuu[0] = rkBP.m_aakXuu[0];
        kSubBP.m_aakXuu[1] = kXuu01;
        kSubBP.m_aakXuu[2] = kXuu02;
        kSubBP.m_aakXvv[0] = rkBP.m_aakXvv[0];
        kSubBP.m_aakXvv[1] = kXvv01;
        kSubBP.m_aakXvv[2] = kXvv02;
        kSubBP.m_aakXhh[0] = rkBP.m_aakXhh[0];
        kSubBP.m_aakXhh[1] = kXhh01;
        kSubBP.m_aakXhh[2] = kXhh02;

        SubdivideUR(iLevel,fDSqr,akX,akXu,akXv,i0,i01,i02,kSubBP);

        // subtriangle <(sm,tm),(s1,tm),(sm,t1)>
        kSubBP.m_aakXuu[0] = kXuu02;
        kSubBP.m_aakXuu[1] = kXuu01;
        kSubBP.m_aakXuu[2] = kXuu12;
        kSubBP.m_aakXvv[0] = kXvv02;
        kSubBP.m_aakXvv[1] = kXvv01;
        kSubBP.m_aakXvv[2] = kXvv12;
        kSubBP.m_aakXhh[0] = kXhh02;
        kSubBP.m_aakXhh[1] = kXhh01;
        kSubBP.m_aakXhh[2] = kXhh12;

        SubdivideLL(iLevel,fDSqr,akX,akXu,akXv,i02,i01,i12,kSubBP);
    }
}
//----------------------------------------------------------------------------

