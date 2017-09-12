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

#include "MgcBezierCylinder3G.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierCylinder3G::BezierCylinder3G (Vector3* akCtrlPoint)
    :
    BezierCylinderG(3,akCtrlPoint)
{
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetPosition (Real fU, Real fV) const
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

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 4; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 4];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetDerivativeU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        -3.0f*fOmU2,
        3.0f*fOmU*(fOmU - 2.0f*fU),
        3.0f*fU*(2.0f*fOmU - fU),
        3.0f*fU2
    };

    Real fOmV = 1.0f - fV;

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 4; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 4];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetDerivativeV (Real fU, Real) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        fOmU*fOmU2,
        3.0f*fOmU2*fU,
        3.0f*fOmU*fU2,
        fU*fU2
    };

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 4; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 4];
    }

    Vector3 kResult = kB1 - kB0;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetDerivativeUU (Real fU, Real fV) const
{
    Real fOmU = 1.0f - fU;
    Real afUCoeff[4] =
    {
        6.0f*fOmU,
        6.0f*(fU - 2.0f*fOmU),
        6.0f*(fOmU - 2.0f*fU),
        6.0f*fU
    };

    Real fOmV = 1.0f - fV;

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 4; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 4];
    }

    Vector3 kResult = fOmV*kB0 + fV*kB1;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetDerivativeUV (Real fU, Real) const
{
    Real fOmU = 1.0f - fU, fU2 = fU*fU, fOmU2 = fOmU*fOmU;
    Real afUCoeff[4] =
    {
        -3.0f*fOmU2,
        3.0f*fOmU*(fOmU - 2.0f*fU),
        3.0f*fU*(2.0f*fOmU - fU),
        3.0f*fU2
    };

    Vector3 kB0 = Vector3::ZERO, kB1 = Vector3::ZERO;
    for (int iX = 0; iX < 4; iX++)
    {
        kB0 += afUCoeff[iX]*m_akCtrlPoint[iX];
        kB1 += afUCoeff[iX]*m_akCtrlPoint[iX + 4];
    }

    Vector3 kResult = kB1 - kB0;
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 BezierCylinder3G::GetDerivativeVV (Real, Real) const
{
    return Vector3::ZERO;
}
//----------------------------------------------------------------------------
void BezierCylinder3G::Tessellate (int iLevel, bool bWantNormals)
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
    akX[kIP.m_i01] = m_akCtrlPoint[4];
    akX[kIP.m_i10] = m_akCtrlPoint[3];
    akX[kIP.m_i11] = m_akCtrlPoint[7];

    // derivatives for subdivision (for normal vectors)
    Vector3* akXu;
    Vector3* akXv;
    if ( bWantNormals )
    {
        akXu = new Vector3[m_iVertexQuantity];
        akXu[kIP.m_i00] = 3.0f*(m_akCtrlPoint[1] - m_akCtrlPoint[0]);
        akXu[kIP.m_i01] = 3.0f*(m_akCtrlPoint[5] - m_akCtrlPoint[4]);
        akXu[kIP.m_i10] = 3.0f*(m_akCtrlPoint[3] - m_akCtrlPoint[2]);
        akXu[kIP.m_i11] = 3.0f*(m_akCtrlPoint[7] - m_akCtrlPoint[6]);

        akXv = new Vector3[m_iVertexQuantity];
        akXv[kIP.m_i00] = 3.0f*(m_akCtrlPoint[4] - m_akCtrlPoint[0]);
        akXv[kIP.m_i01] = akXv[kIP.m_i00];
        akXv[kIP.m_i10] = 3.0f*(m_akCtrlPoint[7] - m_akCtrlPoint[3]);
        akXv[kIP.m_i11] = akXv[kIP.m_i10];
    }
    else
    {
        akXu = NULL;
        akXv = NULL;
    }

    // recursive subdivision
    Vector3 akXuuu[2];
    if ( iLevel > 0 || m_iCylinderLevel > 0 )
    {
        kIP.m_aakXuu[0][0] = 6.0f*(m_akCtrlPoint[0] - 2.0f*m_akCtrlPoint[1] +
            m_akCtrlPoint[2]);
        kIP.m_aakXuu[0][1] = 6.0f*(m_akCtrlPoint[4] - 2.0f*m_akCtrlPoint[5] +
            m_akCtrlPoint[6]);
        kIP.m_aakXuu[1][0] = 6.0f*(m_akCtrlPoint[1] - 2.0f*m_akCtrlPoint[2] +
            m_akCtrlPoint[3]);
        kIP.m_aakXuu[1][1] = 6.0f*(m_akCtrlPoint[5] - 2.0f*m_akCtrlPoint[6] +
            m_akCtrlPoint[7]);

        if ( akXu )
        {
            akXuuu[0] = 6.0f*(m_akCtrlPoint[3] + 3.0f*(m_akCtrlPoint[1] -
                m_akCtrlPoint[2]) - m_akCtrlPoint[0]);
            akXuuu[1] = 6.0f*(m_akCtrlPoint[7] + 3.0f*(m_akCtrlPoint[5] -
                m_akCtrlPoint[6]) - m_akCtrlPoint[4]);
        }
    }

    if ( iLevel > 0 )
    {
        SubdivideBoundary(--iLevel,0.25f,akX,akXu,akXv,akXuuu,kIP);
    }

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
void BezierCylinder3G::SubdivideBoundary (int iLevel, Real fDSqr,
    Vector3* akX, Vector3* akXu, Vector3* akXv, Vector3 akXuuu[2],
    IntervalParameters& rkIP)
{
    // subdivision indices
    int iM0 = (rkIP.m_i00 + rkIP.m_i10) >> 1;
    int iM1 = (rkIP.m_i01 + rkIP.m_i11) >> 1;

    // vertices
    Vector3 kXssM0 = 0.5f*(rkIP.m_aakXuu[0][0] + rkIP.m_aakXuu[1][0]);
    Vector3 kXssM1 = 0.5f*(rkIP.m_aakXuu[0][1] + rkIP.m_aakXuu[1][1]);
    akX[iM0] = 0.5f*(akX[rkIP.m_i00] + akX[rkIP.m_i10] - fDSqr*kXssM0);
    akX[iM1] = 0.5f*(akX[rkIP.m_i01] + akX[rkIP.m_i11] - fDSqr*kXssM1);

    // derivatives (for normal vectors)
    if ( akXu )
    {
        akXu[iM0] = 0.5f*(akXu[rkIP.m_i00] + akXu[rkIP.m_i10] -
            fDSqr*akXuuu[0]);
        akXu[iM1] = 0.5f*(akXu[rkIP.m_i01] + akXu[rkIP.m_i11] -
            fDSqr*akXuuu[1]);
        akXv[iM0] = akX[iM1] - akX[iM0];
        akXv[iM1] = akXv[iM0];
    }

    // recurse on two children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        IntervalParameters kSubIP;

        // subinterval [u0,uM]
        kSubIP.m_i00 = rkIP.m_i00;
        kSubIP.m_i01 = rkIP.m_i01;
        kSubIP.m_i10 = iM0;
        kSubIP.m_i11 = iM1;

        kSubIP.m_aakXuu[0][0] = rkIP.m_aakXuu[0][0];
        kSubIP.m_aakXuu[0][1] = rkIP.m_aakXuu[0][1];
        kSubIP.m_aakXuu[1][0] = kXssM0;
        kSubIP.m_aakXuu[1][1] = kXssM1;

        SubdivideBoundary(iLevel,fDSqr,akX,akXu,akXv,akXuuu,kSubIP);

        // subinterval [uM,u1]
        kSubIP.m_i00 = iM0;
        kSubIP.m_i01 = iM1;
        kSubIP.m_i10 = rkIP.m_i10;
        kSubIP.m_i11 = rkIP.m_i11;

        kSubIP.m_aakXuu[0][0] = kXssM0;
        kSubIP.m_aakXuu[0][1] = kXssM1;
        kSubIP.m_aakXuu[1][0] = rkIP.m_aakXuu[1][0];
        kSubIP.m_aakXuu[1][1] = rkIP.m_aakXuu[1][1];

        SubdivideBoundary(iLevel,fDSqr,akX,akXu,akXv,akXuuu,kSubIP);
    }
}
//----------------------------------------------------------------------------
void BezierCylinder3G::SubdivideCylinder (int iCLevel, Vector3* akX,
    Vector3* akXu, Vector3* akXv, int i0, int i1, int iTwoPowL)
{
    // subdivision index
    int iM = (i0 + i1) >> 1;

    int j0 = i0, jM = iM, j1 = i1;
    int iJLast = jM + iTwoPowL;
    for (/**/; jM <= iJLast; j0++, jM++, j1++)
    {
        // vertices
        akX[jM] = 0.5f*(akX[j0] + akX[j1]);

        // derivatives (for normal vectors)
        if ( akXu )
        {
            akXu[jM] = 0.5f*(akXu[j0] + akXu[j1]);
            akXv[jM] = akXv[j0];
        }
    }

    // recurse on two children
    if ( iCLevel > 0 )
    {
        iCLevel--;

        // subinterval [v0,vM]
        SubdivideCylinder(iCLevel,akX,akXu,akXv,i0,iM,iTwoPowL);

        // subinterval [vM,v1]
        SubdivideCylinder(iCLevel,akX,akXu,akXv,iM,i1,iTwoPowL);
    }
}
//----------------------------------------------------------------------------

