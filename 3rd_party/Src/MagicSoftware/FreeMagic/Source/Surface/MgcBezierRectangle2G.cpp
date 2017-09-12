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

#include "MgcBezierRectangle2G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierRectangle2G::BezierRectangle2G (Vector3* akCtrlPoint)
    :
    BezierRectangleG(2,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetPosition (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        fOmU*fOmU,
        2.0f*fOmU*fU,
        fU*fU
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[3] =
    {
        fOmV*fOmV,
        2.0f*fOmV*fV,
        fV*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetDerivativeU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        -2.0f*fOmU,
        2.0f*(fOmU - fU),
        2.0f*fU
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[3] =
    {
        fOmV*fOmV,
        2.0f*fOmV*fV,
        fV*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetDerivativeV (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        fOmU*fOmU,
        2.0f*fOmU*fU,
        fU*fU
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[3] =
    {
        -2.0f*fOmV,
        2.0f*(fOmV - fV),
        2.0f*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetDerivativeUU (Real, Real fV) const
{
    Real afUCoeff[3] =
    {
        2.0f,
        -4.0f,
        2.0f
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[3] =
    {
        fOmV*fOmV,
        2.0f*fOmV*fV,
        fV*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetDerivativeUV (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        -2.0f*fOmU,
        2.0f*(fOmU - fU),
        2.0f*fU
    };

    Real fOmV = 1.0f - fV;
    Real afVCoeff[3] =
    {
        -2.0f*fOmV,
        2.0f*(fOmV - fV),
        2.0f*fV
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierRectangle2G::GetDerivativeVV (Real fU, Real) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        fOmU*fOmU,
        2.0f*fOmU*fU,
        fU*fU
    };

    Real afVCoeff[3] =
    {
        2.0f,
        -4.0f,
        2.0f
    };

    Vector3 kResult = Vector3::ZERO;
    for (int iY = 0; iY < 3; iY++)
    {
        for (int iX = 0; iX < 3; iX++)
            kResult += afUCoeff[iX]*afVCoeff[iY]*m_akCtrlPoint[iX + 3*iY];
    }

    return kResult;
}
//----------------------------------------------------------------------------
void BezierRectangle2G::Tessellate (int iLevel, bool bWantNormals)
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
    akX[kBP.m_i01] = m_akCtrlPoint[6];
    akX[kBP.m_i10] = m_akCtrlPoint[2];
    akX[kBP.m_i11] = m_akCtrlPoint[8];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[kBP.m_i00] = 3.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[kBP.m_i01] = 3.0f*(m_akCtrlPoint[7] - m_akCtrlPoint[6]);
        akXu[kBP.m_i10] = 3.0f*(m_akCtrlPoint[2] - m_akCtrlPoint[1]);
        akXu[kBP.m_i11] = 3.0f*(m_akCtrlPoint[8] - m_akCtrlPoint[7]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[kBP.m_i00] = 3.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[0]);
        akXv[kBP.m_i01] = 3.0f*(m_akCtrlPoint[6] - m_akCtrlPoint[3]);
        akXv[kBP.m_i10] = 3.0f*(m_akCtrlPoint[5] - m_akCtrlPoint[2]);
        akXv[kBP.m_i11] = 3.0f*(m_akCtrlPoint[8] - m_akCtrlPoint[5]);
    }
    else
    {
        akXu = NULL;
        akXv = NULL;
    }

    // recursive subdivision
    if ( iLevel > 0 )
    {
        kBP.m_akXuu[0] = 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] +
            m_akCtrlPoint[2]);
        kBP.m_akXuu[1] = 2.0f*(m_akCtrlPoint[6] - 2.0f*m_akCtrlPoint[7] +
            m_akCtrlPoint[8]);
        kBP.m_akXvv[0] = 2.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[3] +
            m_akCtrlPoint[6]);
        kBP.m_akXvv[1] = 2.0f*(m_akCtrlPoint[2] - 2.0f*m_akCtrlPoint[5] +
            m_akCtrlPoint[8]);

        Vector3 kXuuvv = 4.0f*(m_akCtrlPoint[0] + m_akCtrlPoint[2] +
            m_akCtrlPoint[6] + m_akCtrlPoint[8] - 2.0f*(m_akCtrlPoint[1] +
            m_akCtrlPoint[3] + m_akCtrlPoint[5] + m_akCtrlPoint[7]) +
            4.0f*m_akCtrlPoint[4]);

        if ( bWantNormals )
        {
            kBP.m_akXuuv[0] = 4.0f*(m_akCtrlPoint[3] + m_akCtrlPoint[5] -
                m_akCtrlPoint[0] - m_akCtrlPoint[2] + 2.0f*(m_akCtrlPoint[1] -
                m_akCtrlPoint[4]));
            kBP.m_akXuuv[1] = 4.0f*(m_akCtrlPoint[6] + m_akCtrlPoint[8] -
                m_akCtrlPoint[3] - m_akCtrlPoint[5] + 2.0f*(m_akCtrlPoint[4] -
                m_akCtrlPoint[7]));
            kBP.m_akXuvv[0] = 4.0f*(m_akCtrlPoint[1] + m_akCtrlPoint[7] -
                m_akCtrlPoint[0] - m_akCtrlPoint[6] + 2.0f*(m_akCtrlPoint[3] -
                m_akCtrlPoint[4]));
            kBP.m_akXuvv[1] = 4.0f*(m_akCtrlPoint[2] + m_akCtrlPoint[8] - 
                m_akCtrlPoint[1] - m_akCtrlPoint[7] + 2.0f*(m_akCtrlPoint[4] -
                m_akCtrlPoint[5]));
        }

        Subdivide(--iLevel,0.25f,akX,akXu,akXv,kXuuvv,kBP);
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
void BezierRectangle2G::Subdivide (int iLevel, Real fDSqr, Vector3* akX,
    Vector3* akXu, Vector3* akXv, Vector3& rkXuuvv, BlockParameters& rkBP)
{
    // subdivision indices
    int iIM0 = (rkBP.m_i00 + rkBP.m_i10) >> 1;
    int iIM1 = (rkBP.m_i01 + rkBP.m_i11) >> 1;
    int iI0M = (rkBP.m_i00 + rkBP.m_i01) >> 1;
    int iI1M = (rkBP.m_i10 + rkBP.m_i11) >> 1;
    int iIMM = (iI0M + iI1M) >> 1;

    // vertices

    // top and bottom u-edge subdivision
    Vector3 kXvvM0 = 0.5f*(rkBP.m_akXvv[0]+rkBP.m_akXvv[1]-fDSqr*rkXuuvv);
    akX[iIM0] = 0.5f*(akX[rkBP.m_i00] + akX[rkBP.m_i10] -
        fDSqr*rkBP.m_akXuu[0]);
    akX[iIM1] = 0.5f*(akX[rkBP.m_i01] + akX[rkBP.m_i11] -
        fDSqr*rkBP.m_akXuu[1]);

    // left and right v-edge subdivision
    Vector3 kXuu0M = 0.5f*(rkBP.m_akXuu[0]+rkBP.m_akXuu[1]-fDSqr*rkXuuvv);
    akX[iI0M] = 0.5f*(akX[rkBP.m_i00] + akX[rkBP.m_i01] -
        fDSqr*rkBP.m_akXvv[0]);
    akX[iI1M] = 0.5f*(akX[rkBP.m_i10] + akX[rkBP.m_i11] -
        fDSqr*rkBP.m_akXvv[1]);

    // center subdivision
    akX[iIMM] = 0.5f*(akX[iI0M] + akX[iI1M] - fDSqr*kXuu0M);

    // derivatives (for normal vectors)
    Vector3 kXuuv0M, kXuvvM0;
    if ( akXu )
    {
        // top and bottom u-edge subdivision
        akXu[iIM0] = 0.5f*(akXu[rkBP.m_i00] + akXu[rkBP.m_i10]);
        akXv[iIM0] = 0.5f*(akXv[rkBP.m_i00] + akXv[rkBP.m_i10] -
            fDSqr*rkBP.m_akXuuv[0]);
        akXu[iIM1] = 0.5f*(akXu[rkBP.m_i01] + akXu[rkBP.m_i11]);
        akXv[iIM1] = 0.5f*(akXv[rkBP.m_i01] + akXv[rkBP.m_i11] -
            fDSqr*rkBP.m_akXuuv[1]);

        kXuvvM0 = 0.5f*(rkBP.m_akXuvv[0] + rkBP.m_akXuvv[1]);

        // left and right v-edge subdivision
        akXu[iI0M] = 0.5f*(akXu[rkBP.m_i00] + akXu[rkBP.m_i01] -
            fDSqr*rkBP.m_akXuvv[0]);
        akXv[iI0M] = 0.5f*(akXv[rkBP.m_i00] + akXv[rkBP.m_i01]);
        akXu[iI1M] = 0.5f*(akXu[rkBP.m_i10] + akXu[rkBP.m_i11] -
            fDSqr*rkBP.m_akXuvv[1]);
        akXv[iI1M] = 0.5f*(akXv[rkBP.m_i10] + akXv[rkBP.m_i11]);

        kXuuv0M = 0.5f*(rkBP.m_akXuuv[0] + rkBP.m_akXuuv[1]);

        // center subdivision
        akXu[iIMM] = 0.5f*(akXu[iI0M] + akXu[iI1M]);
        akXv[iIMM] = 0.5f*(akXv[iIM0] + akXv[iIM1]);
    }

    // recurse on four children
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

        kSubBP.m_akXuu[0] = rkBP.m_akXuu[0];
        kSubBP.m_akXuu[1] = kXuu0M;
        kSubBP.m_akXvv[0] = rkBP.m_akXvv[0];
        kSubBP.m_akXvv[1] = kXvvM0;

        if ( akXu )
        {
            kSubBP.m_akXuuv[0] = rkBP.m_akXuuv[0];
            kSubBP.m_akXuuv[1] = kXuuv0M;
            kSubBP.m_akXuvv[0] = rkBP.m_akXuvv[0];
            kSubBP.m_akXuvv[1] = kXuvvM0;
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,rkXuuvv,kSubBP);

        // subblock [u0,uM]x[vM,v1]
        kSubBP.m_i00 = iI0M;
        kSubBP.m_i01 = rkBP.m_i01;
        kSubBP.m_i10 = iIMM;
        kSubBP.m_i11 = iIM1;

        kSubBP.m_akXuu[0] = kXuu0M;
        kSubBP.m_akXuu[1] = rkBP.m_akXuu[1];
        kSubBP.m_akXvv[0] = rkBP.m_akXvv[0];
        kSubBP.m_akXvv[1] = kXvvM0;

        if ( akXu )
        {
            kSubBP.m_akXuuv[0] = kXuuv0M;
            kSubBP.m_akXuuv[1] = rkBP.m_akXuuv[1];
            kSubBP.m_akXuvv[0] = rkBP.m_akXuvv[0];
            kSubBP.m_akXuvv[1] = kXuvvM0;
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,rkXuuvv,kSubBP);

        // subblock [uM,u1]x[v0,vM]
        kSubBP.m_i00 = iIM0;
        kSubBP.m_i01 = iIMM;
        kSubBP.m_i10 = rkBP.m_i10;
        kSubBP.m_i11 = iI1M;

        kSubBP.m_akXuu[0] = rkBP.m_akXuu[0];
        kSubBP.m_akXuu[1] = kXuu0M;
        kSubBP.m_akXvv[0] = kXvvM0;
        kSubBP.m_akXvv[1] = rkBP.m_akXvv[1];

        if ( akXu )
        {
            kSubBP.m_akXuuv[0] = rkBP.m_akXuuv[0];
            kSubBP.m_akXuuv[1] = kXuuv0M;
            kSubBP.m_akXuvv[0] = kXuvvM0;
            kSubBP.m_akXuvv[1] = rkBP.m_akXuvv[1];
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,rkXuuvv,kSubBP);

        // subblock [uM,u1]x[vM,v1]
        kSubBP.m_i00 = iIMM;
        kSubBP.m_i01 = iIM1;
        kSubBP.m_i10 = iI1M;
        kSubBP.m_i11 = rkBP.m_i11;

        kSubBP.m_akXuu[0] = kXuu0M;
        kSubBP.m_akXuu[1] = rkBP.m_akXuu[1];
        kSubBP.m_akXvv[0] = kXvvM0;
        kSubBP.m_akXvv[1] = rkBP.m_akXvv[1];

        if ( akXu )
        {
            kSubBP.m_akXuuv[0] = kXuuv0M;
            kSubBP.m_akXuuv[1] = rkBP.m_akXuuv[1];
            kSubBP.m_akXuvv[0] = kXuvvM0;
            kSubBP.m_akXuvv[1] = rkBP.m_akXuvv[1];
        }

        Subdivide(iLevel,fDSqr,akX,akXu,akXv,rkXuuvv,kSubBP);
    }
}
//----------------------------------------------------------------------------

