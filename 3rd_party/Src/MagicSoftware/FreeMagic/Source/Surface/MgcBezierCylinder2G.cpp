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

#include "MgcBezierCylinder2G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierCylinder2G::BezierCylinder2G (Vector3* akCtrlPoint)
    :
    BezierCylinderG(2,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetPosition (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        fOmU*fOmU,
        2.0f*fOmU*fU,
        fU*fU
    };

    Real fOmV = 1.0f - fV;

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 3; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 3];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetDerivativeU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        -2.0f*fOmU,
        2.0f*(fOmU - fU),
        2.0f*fU
    };

    Real fOmV = 1.0f - fV;

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 3; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 3];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetDerivativeV (Real fU, Real) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        fOmU*fOmU,
        2.0f*fOmU*fU,
        fU*fU
    };

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 3; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 3];
    }

    Vector3 kResult = kB1 - kB0;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetDerivativeUU (Real, Real fV) const
{
    Real afUCoeff[3] =
    {
        2.0f,
        -4.0f,
        2.0f
    };

    Real fOmV = 1.0f - fV;

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 3; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 3];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetDerivativeUV (Real fU, Real) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[3] =
    {
        -2.0f*fOmU,
        2.0f*(fOmU - fU),
        2.0f*fU
    };

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 3; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 3];
    }

    Vector3 kResult = kB1 - kB0;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder2G::GetDerivativeVV (Real, Real) const
{
    return Vector3::ZERO;
}
//----------------------------------------------------------------------------
void BezierCylinder2G::Tessellate (int iLevel, bool bWantNormals)
{
    // allocate arrays and compute connectivity
    Initialize(iLevel,bWantNormals);

    // indices of four corners of patch, I[u][v]
    int iTwoPowL = (1 << iLevel);
    int iTwoPowC = (1 << m_iCylinderLevel);
    IntervalParameters kIP;
    kIP.m_i00 = 0;
    kIP.m_i01 = iTwoPowC*(iTwoPowL + 1);
    kIP.m_i10 = iTwoPowL;
    kIP.m_i11 = kIP.m_i01 + iTwoPowL;

    // vertices for subdivision
    Vector3* akX = m_akVertex;
    akX[kIP.m_i00] = m_akCtrlPoint[0];
    akX[kIP.m_i01] = m_akCtrlPoint[3];
    akX[kIP.m_i10] = m_akCtrlPoint[2];
    akX[kIP.m_i11] = m_akCtrlPoint[5];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[kIP.m_i00] = 2.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[kIP.m_i01] = 2.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[3]);
        akXu[kIP.m_i10] = 2.0f*(m_akCtrlPoint[2] - m_akCtrlPoint[1]);
        akXu[kIP.m_i11] = 2.0f*(m_akCtrlPoint[5] - m_akCtrlPoint[4]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[kIP.m_i00] = 2.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[0]);
        akXv[kIP.m_i01] = akXv[kIP.m_i00];
        akXv[kIP.m_i10] = 2.0f*(m_akCtrlPoint[5] - m_akCtrlPoint[2]);
        akXv[kIP.m_i11] = akXv[kIP.m_i10];
    }
    else
    {
        akXu = NULL;
        akXv = NULL;
    }

    // recursive subdivision
    if ( iLevel > 0 || m_iCylinderLevel > 0 )
    {
        kIP.m_aakXuu[0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] +
            m_akCtrlPoint[2]);
        kIP.m_aakXuu[1] = 6.0f*(m_akCtrlPoint[3] - 2.0f*m_akCtrlPoint[4] +
            m_akCtrlPoint[5]);
    }

    if ( iLevel > 0 )
        SubdivideBoundary(--iLevel,0.25,akX,akXu,akXv,kIP);

    if ( m_iCylinderLevel > 0 )
    {
        SubdivideCylinder(m_iCylinderLevel-1,akX,akXu,akXv,0,kIP.m_i01,
            iTwoPowL);
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
void BezierCylinder2G::SubdivideBoundary (int iLevel, Real fDSqr,
    Vector3* akX, Vector3* akXu, Vector3* akXv, IntervalParameters& rkIP)
{
    // subdivision indices
    int iIM0 = (rkIP.m_i00 + rkIP.m_i10) >> 1;
    int iIM1 = (rkIP.m_i01 + rkIP.m_i11) >> 1;

    // vertices
    akX[iIM0] = 0.5f*(akX[rkIP.m_i00] + akX[rkIP.m_i10] -
        fDSqr*rkIP.m_aakXuu[0]);
    akX[iIM1] = 0.5f*(akX[rkIP.m_i01] + akX[rkIP.m_i11] -
        fDSqr*rkIP.m_aakXuu[1]);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        akXu[iIM0] = 0.5f*(akXu[rkIP.m_i00] + akXu[rkIP.m_i10]);
        akXu[iIM1] = 0.5f*(akXu[rkIP.m_i01] + akXu[rkIP.m_i11]);
        akXv[iIM0] = akX[iIM1] - akX[iIM0];
        akXv[iIM1] = akXv[iIM0];
    }

    // recurse on two children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        IntervalParameters kSubIP;

        // subinterval [s0,sM]
        kSubIP.m_i00 = rkIP.m_i00;
        kSubIP.m_i01 = rkIP.m_i01;
        kSubIP.m_i10 = iIM0;
        kSubIP.m_i11 = iIM1;

        kSubIP.m_aakXuu[0] = rkIP.m_aakXuu[0];
        kSubIP.m_aakXuu[1] = rkIP.m_aakXuu[1];

        SubdivideBoundary(iLevel,fDSqr,akX,akXu,akXv,kSubIP);

        // subinterval [sM,s1]
        kSubIP.m_i00 = iIM0;
        kSubIP.m_i01 = iIM1;
        kSubIP.m_i10 = rkIP.m_i10;
        kSubIP.m_i11 = rkIP.m_i11;

        kSubIP.m_aakXuu[0] = rkIP.m_aakXuu[0];
        kSubIP.m_aakXuu[1] = rkIP.m_aakXuu[1];

        SubdivideBoundary(iLevel,fDSqr,akX,akXu,akXv,kSubIP);
    }
}
//----------------------------------------------------------------------------
void BezierCylinder2G::SubdivideCylinder (int iCLevel, Vector3* akX,
    Vector3* akXu, Vector3* akXv, int i0, int i1, int iTwoPowL)
{
    // subdivision index
    int iM = (i0 + i1) >> 1;

    int j0 = i0, jM = iM, j1 = i1;
    int jLast = jM + iTwoPowL;
    for (/**/; jM <= jLast; j0++, jM++, j1++)
    {
        // vertices
        akX[jM] = 0.5f*(akX[j0] + akX[j1]);

        // derivatives (for normal vectors)
        if ( akXu )
        {
            akXu[jM] = 0.5*(akXu[j0] + akXu[j1]);
            akXv[jM] = akXv[j0];
        }
    }

    // recurse on two children
    if ( iCLevel > 0 )
    {
        iCLevel--;

        // subinterval [t0,tM]
        SubdivideCylinder(iCLevel,akX,akXu,akXv,i0,iM,iTwoPowL);

        // subinterval [tM,t1]
        SubdivideCylinder(iCLevel,akX,akXu,akXv,iM,i1,iTwoPowL);
    }
}
//----------------------------------------------------------------------------

