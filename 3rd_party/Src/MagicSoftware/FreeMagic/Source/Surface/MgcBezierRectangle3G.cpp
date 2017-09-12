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

#include "MgcBezierRectangle3G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierRectangle3G::BezierRectangle3G (Vector3* akCtrlPoint)
    :
    BezierRectangleG(3,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetPosition (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        fOmU*fOmU2,
        3.0f*fOmU2*fU,
        3.0f*fOmU*fU2,
        fU*fU2
    };

    Real fOmV = 1.0f - fV, fV2 = fV*fV, fOmV2 = fOmV*fOmV;
    Real afVCoeff[4] =
    {
        fOmV*fOmV2,
        3.0f*fOmV2*fV,
        3.0f*fOmV*fV2,
        fV*fV2
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetDerivativeU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        -3.0f*fOmU2,
        3.0f*fOmU*(fOmU - 2.0f*fU),
        3.0f*fU*(2.0f*fOmU - fU),
        3.0f*fU2
    };

    Real fOmV = 1.0f - fV, fV2 = fV*fV, fOmV2 = fOmV*fOmV;
    Real afVCoeff[4] =
    {
        fOmV*fOmV2,
        3.0f*fOmV2*fV,
        3.0f*fOmV*fV2,
        fV*fV2
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetDerivativeV (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        fOmU*fOmU2,
        3.0f*fOmU2*fU,
        3.0f*fOmU*fU2,
        fU*fU2
    };

    Real fOmV = 1.0f - fV, fV2 = fV*fV, fOmV2 = fOmV*fOmV;
    Real afVCoeff[4] =
    {
        -3.0f*fOmV2,
        3.0f*fOmV*(fOmV - 2.0f*fV),
        3.0f*fV*(2.0f*fOmV - fV),
        3.0f*fV2
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetDerivativeUU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[4] =
    {
        6.0f*fOmU,
        6.0f*(fU - 2.0f*fOmU),
        6.0f*(fOmU - 2.0f*fU),
        6.0f*fU
    };

    Real fOmV = 1.0f - fV, fV2 = fV*fV, fOmV2 = fOmV*fOmV;
    Real afVCoeff[4] =
    {
        fOmV*fOmV2,
        3.0f*fOmV2*fV,
        3.0f*fOmV*fV2,
        fV*fV2
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetDerivativeUV (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        -3.0f*fOmU2,
        3.0f*fOmU*(fOmU - 2.0f*fU),
        3.0f*fU*(2.0f*fOmU - fU),
        3.0f*fU2
    };

    Real fOmV = 1.0f - fV, fV2 = fV*fV, fOmV2 = fOmV*fOmV;
    Real afVCoeff[4] =
    {
        -3.0f*fOmV2,
        3.0f*fOmV*(fOmV - 2.0f*fV),
        3.0f*fV*(2.0f*fOmV - fV),
        3.0f*fV2
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle3G::GetDerivativeVV (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        fOmU*fOmU2,
        3.0f*fOmU2*fU,
        3.0f*fOmU*fU2,
        fU*fU2
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[4] =
    {
        6.0f*fOmV,
        6.0f*(fV - 2.0f*fOmV),
        6.0f*(fOmV - 2.0f*fV),
        6.0f*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 4; iY++)
    {
        for (int iX = 0; iX < 4; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 4*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
void BezierRectangle3G::Tessellate (int iLevel, bool bWantNormals)
{
    // allocate arrays and compute connectivity
    Initialize(iLevel,bWantNormals);

    // indices of four corners of patch, I[u][v]
    int iTwoPowL = (1 << iLevel);
    BlockParameters kBP;
    kBP.m_i00 = 0;
    kBP.m_i01 = iTwoPowL*(iTwoPowL + 1);
    kBP.m_i10 = iTwoPowL;
    kBP.m_i11 = kBP.m_i01 + iTwoPowL;

    // vertices for subdivision
    Vector3* akX = m_akVertex;
    akX[kBP.m_i00] = m_akCtrlPoint[0];
    akX[kBP.m_i01] = m_akCtrlPoint[12];
    akX[kBP.m_i10] = m_akCtrlPoint[3];
    akX[kBP.m_i11] = m_akCtrlPoint[15];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[kBP.m_i00] = 3.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[kBP.m_i01] = 3.0f*(m_akCtrlPoint[13] - m_akCtrlPoint[12]);
        akXu[kBP.m_i10] = 3.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[2]);
        akXu[kBP.m_i11] = 3.0f*(m_akCtrlPoint[15] - m_akCtrlPoint[14]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[kBP.m_i00] = 3.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[0]);
        akXv[kBP.m_i01] = 3.0f*(m_akCtrlPoint[12] - m_akCtrlPoint[8]);
        akXv[kBP.m_i10] = 3.0f*(m_akCtrlPoint[7] - m_akCtrlPoint[3]);
        akXv[kBP.m_i11] = 3.0f*(m_akCtrlPoint[15] - m_akCtrlPoint[11]);
    }
    else
    {
        akXu = NULL;
        akXv = NULL;
    }

    // recursive subdivision
    if ( iLevel > 0 )
    {
        kBP.m_aakXuu[0][0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1]
            + m_akCtrlPoint[2]);
        kBP.m_aakXuu[0][1] = 6.0f*(m_akCtrlPoint[12] - 2.0f*m_akCtrlPoint[13]
            + m_akCtrlPoint[14]);
        kBP.m_aakXuu[1][0] = 6.0f*(m_akCtrlPoint[1] - 2.0f*m_akCtrlPoint[2]
            + m_akCtrlPoint[3]);
        kBP.m_aakXuu[1][1] = 6.0f*(m_akCtrlPoint[13] - 2.0f*m_akCtrlPoint[14]
            + m_akCtrlPoint[15]);
        kBP.m_aakXvv[0][0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[4]
            + m_akCtrlPoint[8]);
        kBP.m_aakXvv[0][1] = 6.0f*(m_akCtrlPoint[4] - 2.0f*m_akCtrlPoint[8]
            + m_akCtrlPoint[12]);
        kBP.m_aakXvv[1][0] = 6.0f*(m_akCtrlPoint[3] - 2.0f*m_akCtrlPoint[7]
            + m_akCtrlPoint[11]);
        kBP.m_aakXvv[1][1] = 6.0f*(m_akCtrlPoint[7] - 2.0f*m_akCtrlPoint[11]
            + m_akCtrlPoint[15]);
        kBP.m_aakXuuvv[0][0] = 36.0f*(m_akCtrlPoint[0] + m_akCtrlPoint[2]
            + m_akCtrlPoint[8] + m_akCtrlPoint[10] -
            2.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[4] + m_akCtrlPoint[6] +
            m_akCtrlPoint[9]) + 4.0f*m_akCtrlPoint[5]);
        kBP.m_aakXuuvv[0][1] = 36.0f*(m_akCtrlPoint[4] + m_akCtrlPoint[6] +
            m_akCtrlPoint[12] + m_akCtrlPoint[14] -
            2.0f*(m_akCtrlPoint[5] + m_akCtrlPoint[8] + m_akCtrlPoint[10] +
            m_akCtrlPoint[13]) + 4.0f*m_akCtrlPoint[9]);
        kBP.m_aakXuuvv[1][0] = 36.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[3] +
            m_akCtrlPoint[9] + m_akCtrlPoint[11] -
            2.0f*(m_akCtrlPoint[2] + m_akCtrlPoint[5] + m_akCtrlPoint[7] +
            m_akCtrlPoint[10]) + 4.0f*m_akCtrlPoint[6]);
        kBP.m_aakXuuvv[1][1] = 36.0f*(m_akCtrlPoint[5] + m_akCtrlPoint[7] +
            m_akCtrlPoint[13] + m_akCtrlPoint[15] -
            2.0f*(m_akCtrlPoint[6] + m_akCtrlPoint[9] + m_akCtrlPoint[11] +
            m_akCtrlPoint[14]) + 4.0f*m_akCtrlPoint[10]);

        if ( bWantNormals )
        {
            kBP.m_aakXuuv[0][0] = 18.0f*(m_akCtrlPoint[4] + m_akCtrlPoint[6] -
                m_akCtrlPoint[0] - m_akCtrlPoint[2] + 2.0f*(m_akCtrlPoint[1] -
                m_akCtrlPoint[5]));
            kBP.m_aakXuuv[0][1] = 18.0f*(m_akCtrlPoint[5] + m_akCtrlPoint[7] -
                m_akCtrlPoint[1] - m_akCtrlPoint[3] + 2.0f*(m_akCtrlPoint[2] -
                m_akCtrlPoint[6]));
            kBP.m_aakXuuv[1][0] = 18.0f*(m_akCtrlPoint[12] + m_akCtrlPoint[14]
                - m_akCtrlPoint[8] - m_akCtrlPoint[10] +
                2.0f*(m_akCtrlPoint[9] - m_akCtrlPoint[13]));
            kBP.m_aakXuuv[1][1] = 18.0f*(m_akCtrlPoint[13] + m_akCtrlPoint[15]
                - m_akCtrlPoint[9] - m_akCtrlPoint[11] +
                2.0f*(m_akCtrlPoint[10] - m_akCtrlPoint[14]));
            kBP.m_aakXuvv[0][0] = 18.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[9] -
                m_akCtrlPoint[0] - m_akCtrlPoint[8] + 2.0f*(m_akCtrlPoint[4] -
                m_akCtrlPoint[5]));
            kBP.m_aakXuvv[0][1] = 18.0f*(m_akCtrlPoint[5] + m_akCtrlPoint[13]
                - m_akCtrlPoint[4] - m_akCtrlPoint[12] +
                2.0f*(m_akCtrlPoint[8] - m_akCtrlPoint[9]));
            kBP.m_aakXuvv[1][0] = 18.0f*(m_akCtrlPoint[3] + m_akCtrlPoint[11]
                - m_akCtrlPoint[2] - m_akCtrlPoint[10] +
                2.0f*(m_akCtrlPoint[6] - m_akCtrlPoint[7]));
            kBP.m_aakXuvv[1][1] = 18.0f*(m_akCtrlPoint[7] + m_akCtrlPoint[15]
                - m_akCtrlPoint[6] - m_akCtrlPoint[14] +
                2.0f*(m_akCtrlPoint[10] - m_akCtrlPoint[11]));
            kBP.m_akXuuu[0] = 6.0f*(m_akCtrlPoint[3] + 3.0f*(m_akCtrlPoint[1]
                - m_akCtrlPoint[2]) - m_akCtrlPoint[0]);
            kBP.m_akXuuu[1] = 6.0f*(m_akCtrlPoint[15] +
                3.0f*(m_akCtrlPoint[13] - m_akCtrlPoint[14]) -
                m_akCtrlPoint[12]);
            kBP.m_akXvvv[0] = 6.0f*(m_akCtrlPoint[12] +
                3.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[8]) -
                m_akCtrlPoint[0]);
            kBP.m_akXvvv[1] = 6.0f*(m_akCtrlPoint[15] +
                3.0f*(m_akCtrlPoint[7] - m_akCtrlPoint[11]) -
                m_akCtrlPoint[3]);
            kBP.m_akXuuuvv[0] = 36.0f*(m_akCtrlPoint[3] + m_akCtrlPoint[11] -
                m_akCtrlPoint[0] - m_akCtrlPoint[8] + 2.0f*(m_akCtrlPoint[4] -
                m_akCtrlPoint[7]) + 3.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[9]
                - m_akCtrlPoint[2] - m_akCtrlPoint[10]) +
                6.0f*(m_akCtrlPoint[6] - m_akCtrlPoint[5]));
            kBP.m_akXuuuvv[1] = 36.0f*(m_akCtrlPoint[7] + m_akCtrlPoint[15] -
                m_akCtrlPoint[4] - m_akCtrlPoint[12] + 2.0f*(m_akCtrlPoint[8]
                - m_akCtrlPoint[11]) + 3.0f*(m_akCtrlPoint[5] +
                m_akCtrlPoint[13] - m_akCtrlPoint[6] - m_akCtrlPoint[14]) +
                6.0f*(m_akCtrlPoint[10] - m_akCtrlPoint[9]));
            kBP.m_akXuuvvv[0] = 36.0f*(m_akCtrlPoint[12] + m_akCtrlPoint[14] -
                m_akCtrlPoint[0] - m_akCtrlPoint[2] + 2.0f*(m_akCtrlPoint[1] -
                m_akCtrlPoint[13]) + 3.0f*(m_akCtrlPoint[4] + m_akCtrlPoint[6]
                - m_akCtrlPoint[8] - m_akCtrlPoint[10]) +
                6.0f*(m_akCtrlPoint[9] - m_akCtrlPoint[5]));
            kBP.m_akXuuvvv[1] = 36.0f*(m_akCtrlPoint[13] + m_akCtrlPoint[15] -
                m_akCtrlPoint[1] - m_akCtrlPoint[3] + 2.0f*(m_akCtrlPoint[2] -
                m_akCtrlPoint[14]) + 3.0f*(m_akCtrlPoint[5] + m_akCtrlPoint[7]
                - m_akCtrlPoint[9] - m_akCtrlPoint[11]) +
                6.0f*(m_akCtrlPoint[10] - m_akCtrlPoint[6]));
        }

        Subdivide(--iLevel,0.25f,akX,akXu,akXv,kBP);
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
void BezierRectangle3G::Subdivide (int iLevel, Real fDSqr, Vector3* akX,
    Vector3* akXu, Vector3* akXv, BlockParameters& rkBP)
{
    // subdivision indices
    unsigned int iIM0 = (rkBP.m_i00 + rkBP.m_i10) >> 1;
    unsigned int iIM1 = (rkBP.m_i01 + rkBP.m_i11) >> 1;
    unsigned int iI0M = (rkBP.m_i00 + rkBP.m_i01) >> 1;
    unsigned int iI1M = (rkBP.m_i10 + rkBP.m_i11) >> 1;
    unsigned int iIMM = (iI0M + iI1M) >> 1;

    // vertices

    // top and bottom u-edge subdivision
    Vector3 kXuuM0 = 0.5f*(rkBP.m_aakXuu[0][0] + rkBP.m_aakXuu[1][0]);
    Vector3 kXuuM1 = 0.5f*(rkBP.m_aakXuu[0][1] + rkBP.m_aakXuu[1][1]);
    Vector3 kXuuvvM0 = 0.5f*(rkBP.m_aakXuuvv[0][0] + rkBP.m_aakXuuvv[1][0]);
    Vector3 kXuuvvM1 = 0.5f*(rkBP.m_aakXuuvv[0][1] + rkBP.m_aakXuuvv[1][1]);
    Vector3 kXvvM0 = 0.5f*(rkBP.m_aakXvv[0][0] + rkBP.m_aakXvv[1][0] -
        fDSqr*kXuuvvM0);
    Vector3 kXvvM1 = 0.5f*(rkBP.m_aakXvv[0][1] + rkBP.m_aakXvv[1][1] -
        fDSqr*kXuuvvM1);
    akX[iIM0] = 0.5f*(akX[rkBP.m_i00] + akX[rkBP.m_i10] - fDSqr*kXuuM0);
    akX[iIM1] = 0.5f*(akX[rkBP.m_i01] + akX[rkBP.m_i11] - fDSqr*kXuuM1);

    // left and right v-edge subdivision
    Vector3 kXvv0M = 0.5f*(rkBP.m_aakXvv[0][0] + rkBP.m_aakXvv[0][1]);
    Vector3 kXvv1M = 0.5f*(rkBP.m_aakXvv[1][0] + rkBP.m_aakXvv[1][1]);
    Vector3 kXuuvv0M = 0.5f*(rkBP.m_aakXuuvv[0][0] + rkBP.m_aakXuuvv[0][1]);
    Vector3 kXuuvv1M = 0.5f*(rkBP.m_aakXuuvv[1][0] + rkBP.m_aakXuuvv[1][1]);
    Vector3 kXuu0M = 0.5f*(rkBP.m_aakXuu[0][0] + rkBP.m_aakXuu[0][1] -
        fDSqr*kXuuvv0M);
    Vector3 kXuu1M = 0.5f*(rkBP.m_aakXuu[1][0] + rkBP.m_aakXuu[1][1] -
        fDSqr*kXuuvv1M);
    akX[iI0M] = 0.5f*(akX[rkBP.m_i00] + akX[rkBP.m_i01] - fDSqr*kXvv0M);
    akX[iI1M] = 0.5f*(akX[rkBP.m_i10] + akX[rkBP.m_i11] - fDSqr*kXvv1M);

    // center subdivision
    Vector3 kXuuMM = 0.5f*(kXuu0M + kXuu1M);
    Vector3 kXvvMM = 0.5f*(kXvvM0 + kXvvM1);
    Vector3 kXuuvvMM = 0.5f*(kXuuvv0M + kXuuvv1M);
    akX[iIMM] = 0.5f*(akX[iI0M] + akX[iI1M] - fDSqr*kXuuMM);

    // derivatives (for normal vectors)
    Vector3 kXuuu0M, kXuvvM0, kXuvvM1, kXuvv0M, kXuvv1M, kXuuutt0M, kXuvvMM;
    Vector3 kXuuvM0, kXuuvM1, kXuuv0M, kXuuv1M, kXvvvM0, kXuuvvvM0, kXuuvMM;
    if ( akXu )
    {
        // top and bottom u-edge subdivision
        kXuuvM0 = 0.5f*(rkBP.m_aakXuuv[0][0] + rkBP.m_aakXuuv[1][0]);
        kXuuvM1 = 0.5f*(rkBP.m_aakXuuv[0][1] + rkBP.m_aakXuuv[1][1]);
        akXu[iIM0] = 0.5f*(akXu[rkBP.m_i00] + akXu[rkBP.m_i10] -
            fDSqr*rkBP.m_akXuuu[0]);
        akXv[iIM0] = 0.5f*(akXv[rkBP.m_i00] + akXv[rkBP.m_i10] -
            fDSqr*kXuuvM0);
        akXu[iIM1] = 0.5f*(akXu[rkBP.m_i01] + akXu[rkBP.m_i11] -
            fDSqr*rkBP.m_akXuuu[1]);
        akXv[iIM1] = 0.5f*(akXv[rkBP.m_i01] + akXv[rkBP.m_i11] -
            fDSqr*kXuuvM1);

        kXuvvM0 = 0.5f*(rkBP.m_aakXuvv[0][0] + rkBP.m_aakXuvv[1][0] -
            fDSqr*rkBP.m_akXuuuvv[0]);
        kXuuvvvM0 = 0.5f*(rkBP.m_akXuuvvv[0] + rkBP.m_akXuuvvv[1]);
        kXvvvM0 = 0.5f*(rkBP.m_akXvvv[0] + rkBP.m_akXvvv[1] -
            fDSqr*kXuuvvvM0);
        kXuvvM1 = 0.5f*(rkBP.m_aakXuvv[0][1] + rkBP.m_aakXuvv[1][1] -
            fDSqr*rkBP.m_akXuuuvv[1]);

        // left and right v-edge subdivision
        kXuvv0M = 0.5f*(rkBP.m_aakXuvv[0][0] + rkBP.m_aakXuvv[0][1]);
        kXuvv1M = 0.5f*(rkBP.m_aakXuvv[1][0] + rkBP.m_aakXuvv[1][1]);
        akXu[iI0M] = 0.5f*(akXu[rkBP.m_i00] + akXu[rkBP.m_i01] -
            fDSqr*kXuvv0M);
        akXv[iI0M] = 0.5f*(akXv[rkBP.m_i00] + akXv[rkBP.m_i01] -
            fDSqr*rkBP.m_akXvvv[0]);
        akXu[iI1M] = 0.5f*(akXu[rkBP.m_i10] + akXu[rkBP.m_i11] -
            fDSqr*kXuvv1M);
        akXv[iI1M] = 0.5f*(akXv[rkBP.m_i10] + akXv[rkBP.m_i11] -
            fDSqr*rkBP.m_akXvvv[1]);

        kXuuv0M = 0.5f*(rkBP.m_aakXuuv[0][0] + rkBP.m_aakXuuv[0][1] -
            fDSqr*rkBP.m_akXuuvvv[0]);
        kXuuutt0M = 0.5f*(rkBP.m_akXuuuvv[0] + rkBP.m_akXuuuvv[1]);
        kXuuu0M = 0.5f*(rkBP.m_akXuuu[0] + rkBP.m_akXuuu[1] -
            fDSqr*kXuuutt0M);
        kXuuv1M = 0.5f*(rkBP.m_aakXuuv[1][0] + rkBP.m_aakXuuv[1][1] -
            fDSqr*rkBP.m_akXuuvvv[1]);

        // center subdivision
        kXuuvMM = 0.5f*(kXuuv0M + kXuuv1M);
        kXuvvMM = 0.5f*(kXuvvM0 + kXuvvM1);
        akXu[iIMM] = 0.5f*(akXu[iI0M] + akXu[iI1M] - fDSqr*kXuuu0M);
        akXv[iIMM] = 0.5f*(akXv[iIM0] + akXv[iIM1] - fDSqr*kXvvvM0);
    }

    // reurse on four children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        BlockParameters kSubBP;

        // subblock [u0,uM]x[v0,vM]
        kSubBP.m_i00 = rkBP.m_i00;
        kSubBP.m_i01 = iI0M;
        kSubBP.m_i10 = iIM0;
        kSubBP.m_i11 = iIMM;

        kSubBP.m_aakXuu[0][0] = rkBP.m_aakXuu[0][0];
        kSubBP.m_aakXuu[0][1] = kXuu0M;
        kSubBP.m_aakXuu[1][0] = kXuuM0;
        kSubBP.m_aakXuu[1][1] = kXuuMM;
        kSubBP.m_aakXvv[0][0] = rkBP.m_aakXvv[0][0];
        kSubBP.m_aakXvv[0][1] = kXvv0M;
        kSubBP.m_aakXvv[1][0] = kXvvM0;
        kSubBP.m_aakXvv[1][1] = kXvvMM;
        kSubBP.m_aakXuuvv[0][0] = rkBP.m_aakXuuvv[0][0];
        kSubBP.m_aakXuuvv[0][1] = kXuuvv0M;
        kSubBP.m_aakXuuvv[1][0] = kXuuvvM0;
        kSubBP.m_aakXuuvv[1][1] = kXuuvvMM;

        if ( akXu )
        {
            kSubBP.m_akXuuu[0] = rkBP.m_akXuuu[0];
            kSubBP.m_akXuuu[1] = kXuuu0M;
            kSubBP.m_aakXuuv[0][0] = rkBP.m_aakXuuv[0][0];
            kSubBP.m_aakXuuv[0][1] = kXuuv0M;
            kSubBP.m_aakXuuv[1][0] = kXuuvM0;
            kSubBP.m_aakXuuv[1][1] = kXuuvMM;
            kSubBP.m_aakXuvv[0][0] = rkBP.m_aakXuvv[0][0];
            kSubBP.m_aakXuvv[0][1] = kXuvv0M;
            kSubBP.m_aakXuvv[1][0] = kXuvvM0;
            kSubBP.m_aakXuvv[1][1] = kXuvvMM;
            kSubBP.m_akXvvv[0] = rkBP.m_akXvvv[0];
            kSubBP.m_akXvvv[1] = kXvvvM0;
            kSubBP.m_akXuuuvv[0] = rkBP.m_akXuuuvv[0];
            kSubBP.m_akXuuuvv[1] = kXuuutt0M;
            kSubBP.m_akXuuvvv[0] = rkBP.m_akXuuvvv[0];
            kSubBP.m_akXuuvvv[1] = kXuuvvvM0;
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,kSubBP);

        // subblock [u0,uM]x[vM,v1]
        kSubBP.m_i00 = iI0M;
        kSubBP.m_i01 = rkBP.m_i01;
        kSubBP.m_i10 = iIMM;
        kSubBP.m_i11 = iIM1;

        kSubBP.m_aakXuu[0][0] = kXuu0M;
        kSubBP.m_aakXuu[0][1] = rkBP.m_aakXuu[0][1];
        kSubBP.m_aakXuu[1][0] = kXuuMM;
        kSubBP.m_aakXuu[1][1] = kXuuM1;
        kSubBP.m_aakXvv[0][0] = kXvv0M;
        kSubBP.m_aakXvv[0][1] = rkBP.m_aakXvv[0][1];
        kSubBP.m_aakXvv[1][0] = kXvvMM;
        kSubBP.m_aakXvv[1][1] = kXvvM1;
        kSubBP.m_aakXuuvv[0][0] = kXuuvv0M;
        kSubBP.m_aakXuuvv[0][1] = rkBP.m_aakXuuvv[0][1];
        kSubBP.m_aakXuuvv[1][0] = kXuuvvMM;
        kSubBP.m_aakXuuvv[1][1] = kXuuvvM1;

        if ( akXu )
        {
            kSubBP.m_akXuuu[0] = kXuuu0M;
            kSubBP.m_akXuuu[1] = rkBP.m_akXuuu[1];
            kSubBP.m_aakXuuv[0][0] = kXuuv0M;
            kSubBP.m_aakXuuv[0][1] = rkBP.m_aakXuuv[0][1];
            kSubBP.m_aakXuuv[1][0] = kXuuvMM;
            kSubBP.m_aakXuuv[1][1] = kXuuvM1;
            kSubBP.m_aakXuvv[0][0] = kXuvv0M;
            kSubBP.m_aakXuvv[0][1] = rkBP.m_aakXuvv[0][1];
            kSubBP.m_aakXuvv[1][0] = kXuvvMM;
            kSubBP.m_aakXuvv[1][1] = kXuvvM1;
            kSubBP.m_akXvvv[0] = rkBP.m_akXvvv[0];
            kSubBP.m_akXvvv[1] = kXvvvM0;
            kSubBP.m_akXuuuvv[0] = kXuuutt0M;
            kSubBP.m_akXuuuvv[1] = rkBP.m_akXuuuvv[1];
            kSubBP.m_akXuuvvv[0] = rkBP.m_akXuuvvv[0];
            kSubBP.m_akXuuvvv[1] = kXuuvvvM0;
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,kSubBP);

        // subblock [uM,u1]x[v0,vM]
        kSubBP.m_i00 = iIM0;
        kSubBP.m_i01 = iIMM;
        kSubBP.m_i10 = rkBP.m_i10;
        kSubBP.m_i11 = iI1M;

        kSubBP.m_aakXuu[0][0] = kXuuM0;
        kSubBP.m_aakXuu[0][1] = kXuuMM;
        kSubBP.m_aakXuu[1][0] = rkBP.m_aakXuu[1][0];
        kSubBP.m_aakXuu[1][1] = kXuu1M;
        kSubBP.m_aakXvv[0][0] = kXvvM0;
        kSubBP.m_aakXvv[0][1] = kXvvMM;
        kSubBP.m_aakXvv[1][0] = rkBP.m_aakXvv[1][0];
        kSubBP.m_aakXvv[1][1] = kXvv1M;
        kSubBP.m_aakXuuvv[0][0] = kXuuvvM0;
        kSubBP.m_aakXuuvv[0][1] = kXuuvvMM;
        kSubBP.m_aakXuuvv[1][0] = rkBP.m_aakXuuvv[1][0];
        kSubBP.m_aakXuuvv[1][1] = kXuuvv1M;

        if ( akXu )
        {
            kSubBP.m_akXuuu[0] = rkBP.m_akXuuu[0];
            kSubBP.m_akXuuu[1] = kXuuu0M;
            kSubBP.m_aakXuuv[0][0] = kXuuvM0;
            kSubBP.m_aakXuuv[0][1] = kXuuvMM;
            kSubBP.m_aakXuuv[1][0] = rkBP.m_aakXuuv[1][0];
            kSubBP.m_aakXuuv[1][1] = kXuuv1M;
            kSubBP.m_aakXuvv[0][0] = kXuvvM0;
            kSubBP.m_aakXuvv[0][1] = kXuvvMM;
            kSubBP.m_aakXuvv[1][0] = rkBP.m_aakXuvv[1][0];
            kSubBP.m_aakXuvv[1][1] = kXuvv1M;
            kSubBP.m_akXvvv[0] = kXvvvM0;
            kSubBP.m_akXvvv[1] = rkBP.m_akXvvv[1];
            kSubBP.m_akXuuuvv[0] = rkBP.m_akXuuuvv[0];
            kSubBP.m_akXuuuvv[1] = kXuuutt0M;
            kSubBP.m_akXuuvvv[0] = kXuuvvvM0;
            kSubBP.m_akXuuvvv[1] = rkBP.m_akXuuvvv[1];
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,kSubBP);

        // subblock [uM,u1]x[vM,v1]
        kSubBP.m_i00 = iIMM;
        kSubBP.m_i01 = iIM1;
        kSubBP.m_i10 = iI1M;
        kSubBP.m_i11 = rkBP.m_i11;

        kSubBP.m_aakXuu[0][0] = kXuuMM;
        kSubBP.m_aakXuu[0][1] = kXuuM1;
        kSubBP.m_aakXuu[1][0] = kXuu1M;
        kSubBP.m_aakXuu[1][1] = rkBP.m_aakXuu[1][1];
        kSubBP.m_aakXvv[0][0] = kXvvMM;
        kSubBP.m_aakXvv[0][1] = kXvvM1;
        kSubBP.m_aakXvv[1][0] = kXvv1M;
        kSubBP.m_aakXvv[1][1] = rkBP.m_aakXvv[1][1];
        kSubBP.m_aakXuuvv[0][0] = kXuuvvMM;
        kSubBP.m_aakXuuvv[0][1] = kXuuvvM1;
        kSubBP.m_aakXuuvv[1][0] = kXuuvv1M;
        kSubBP.m_aakXuuvv[1][1] = rkBP.m_aakXuuvv[1][1];

        if ( akXu )
        {
            kSubBP.m_akXuuu[0] = kXuuu0M;
            kSubBP.m_akXuuu[1] = rkBP.m_akXuuu[1];
            kSubBP.m_aakXuuv[0][0] = kXuuvMM;
            kSubBP.m_aakXuuv[0][1] = kXuuvM1;
            kSubBP.m_aakXuuv[1][0] = kXuuv1M;
            kSubBP.m_aakXuuv[1][1] = rkBP.m_aakXuuv[1][1];
            kSubBP.m_aakXuvv[0][0] = kXuvvMM;
            kSubBP.m_aakXuvv[0][1] = kXuvvM1;
            kSubBP.m_aakXuvv[1][0] = kXuvv1M;
            kSubBP.m_aakXuvv[1][1] = rkBP.m_aakXuvv[1][1];
            kSubBP.m_akXvvv[0] = kXvvvM0;
            kSubBP.m_akXvvv[1] = rkBP.m_akXvvv[1];
            kSubBP.m_akXuuuvv[0] = kXuuutt0M;
            kSubBP.m_akXuuuvv[1] = rkBP.m_akXuuuvv[1];
            kSubBP.m_akXuuvvv[0] = kXuuvvvM0;
            kSubBP.m_akXuuvvv[1] = rkBP.m_akXuuvvv[1];
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,kSubBP);
    }
}
//----------------------------------------------------------------------------

