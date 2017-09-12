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

#include "MgcIntegrate.h"
#include "MgcLinearSystem.h"
#include "MgcNaturalSpline2.h"
#include "MgcPolynomial.h"
using namespace Mgc;

// This used to be declared in the method GetLengthKey, but MipsPro 7.2.x on
// IRIX core dumps in that case.
class _NaturalSpline2ThisPlusKey
{
public:
    _NaturalSpline2ThisPlusKey (const NaturalSpline2* pkThis, int iKey)
        : m_pkThis(pkThis), m_iKey(iKey) { /**/ }

    const NaturalSpline2* m_pkThis;
    int m_iKey;
};

//----------------------------------------------------------------------------
NaturalSpline2::NaturalSpline2 (BoundaryType eType, int iSegments,
    Real* afTime, Vector2* akPoint)
    :
    MultipleCurve2(iSegments,afTime)
{
    m_akA = akPoint;

    switch ( eType )
    {
        case BT_FREE:
        {
            CreateFreeSpline();
            break;
        }
        case BT_CLAMPED:
        {
            CreateClampedSpline();
            break;
        }
        case BT_CLOSED:
        {
            CreateClosedSpline();
            break;
        }
    }
}
//----------------------------------------------------------------------------
NaturalSpline2::~NaturalSpline2 ()
{
    delete[] m_akA;
    delete[] m_akB;
    delete[] m_akC;
    delete[] m_akD;
}
//----------------------------------------------------------------------------
void NaturalSpline2::CreateFreeSpline ()
{
    Real* afDt = new Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
        afDt[i] = m_afTime[i+1] - m_afTime[i];

    Real* afD2t = new Real[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
        afD2t[i] = m_afTime[i+1] - m_afTime[i-1];

    Vector2* akAlpha = new Vector2[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
    {
        Vector2 kNumer = 3.0f*(afDt[i-1]*m_akA[i+1] - afD2t[i]*m_akA[i] +
            afDt[i]*m_akA[i-1]);
        Real fInvDenom = 1.0f/(afDt[i-1]*afDt[i]);
        akAlpha[i] = fInvDenom*kNumer;
    }

    Real* afEll = new Real[m_iSegments+1];
    Real* afMu = new Real[m_iSegments];
    Vector2* akZ = new Vector2[m_iSegments+1];
    Real fInv;

    afEll[0] = 1.0f;
    afMu[0] = 0.0f;
    akZ[0] = Vector2::ZERO;
    for (i = 1; i < m_iSegments; i++)
    {
        afEll[i] = 2.0f*afD2t[i] - afDt[i-1]*afMu[i-1];
        fInv = 1.0f/afEll[i];
        afMu[i] = fInv*afDt[i];
        akZ[i] = fInv*(akAlpha[i] - afDt[i-1]*akZ[i-1]);
    }
    afEll[m_iSegments] = 1.0f;
    akZ[m_iSegments] = Vector2::ZERO;

    m_akB = new Vector2[m_iSegments];
    m_akC = new Vector2[m_iSegments+1];
    m_akD = new Vector2[m_iSegments];

    m_akC[m_iSegments] = Vector2::ZERO;

    const Real fOneThird = 1.0f/3.0f;
    for (i = m_iSegments-1; i >= 0; i--)
    {
        m_akC[i] = akZ[i] - afMu[i]*m_akC[i+1];
        fInv = 1.0f/afDt[i];
        m_akB[i] = fInv*(m_akA[i+1] - m_akA[i]) - fOneThird*afDt[i]*(
            m_akC[i+1] + 2.0f*m_akC[i]);
        m_akD[i] = fOneThird*fInv*(m_akC[i+1] - m_akC[i]);
    }

    delete[] afDt;
    delete[] afD2t;
    delete[] akAlpha;
    delete[] afEll;
    delete[] afMu;
    delete[] akZ;
}
//----------------------------------------------------------------------------
void NaturalSpline2::CreateClampedSpline ()
{
    Real* afDt = new Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
        afDt[i] = m_afTime[i+1] - m_afTime[i];

    Real* afD2t = new Real[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
        afD2t[i] = m_afTime[i+1] - m_afTime[i-1];

    Vector2* akAlpha = new Vector2[m_iSegments+1];
    Real fInv = 1.0f/afDt[0];
    akAlpha[0] = 3.0f*(fInv - 1.0f)*(m_akA[1] - m_akA[0]);
    fInv = 1.0f/afDt[m_iSegments-1];
    akAlpha[m_iSegments] = 3.0f*(1.0f - fInv)*(m_akA[m_iSegments] -
        m_akA[m_iSegments-1]);
    for (i = 1; i < m_iSegments; i++)
    {
        Vector2 kNumer = 3.0f*(afDt[i-1]*m_akA[i+1] - afD2t[i]*m_akA[i] +
            afDt[i]*m_akA[i-1]);
        Real fInvDenom = 1.0f/(afDt[i-1]*afDt[i]);
        akAlpha[i] = fInvDenom*kNumer;
    }

    Real* afEll = new Real[m_iSegments+1];
    Real* afMu = new Real[m_iSegments];
    Vector2* akZ = new Vector2[m_iSegments+1];

    afEll[0] = 2.0f*afDt[0];
    afMu[0] = 0.5f;
    fInv = 1.0f/afEll[0];
    akZ[0] = fInv*akAlpha[0];

    for (i = 1; i < m_iSegments; i++)
    {
        afEll[i] = 2.0f*afD2t[i] - afDt[i-1]*afMu[i-1];
        fInv = 1.0f/afEll[i];
        afMu[i] = fInv*afDt[i];
        akZ[i] = fInv*(akAlpha[i] - afDt[i-1]*akZ[i-1]);
    }
    afEll[m_iSegments] = afDt[m_iSegments-1]*(2.0f - afMu[m_iSegments-1]);
    fInv = 1.0f/afEll[m_iSegments];
    akZ[m_iSegments] = fInv*(akAlpha[m_iSegments] - afDt[m_iSegments-1]*
        akZ[m_iSegments-1]);

    m_akB = new Vector2[m_iSegments];
    m_akC = new Vector2[m_iSegments+1];
    m_akD = new Vector2[m_iSegments];

    m_akC[m_iSegments] = akZ[m_iSegments];

    const Real fOneThird = 1.0f/3.0f;
    for (i = m_iSegments-1; i >= 0; i--)
    {
        m_akC[i] = akZ[i] - afMu[i]*m_akC[i+1];
        fInv = 1.0f/afDt[i];
        m_akB[i] = fInv*(m_akA[i+1] - m_akA[i]) - fOneThird*afDt[i]*(
            m_akC[i+1] + 2.0f*m_akC[i]);
        m_akD[i] = fOneThird*fInv*(m_akC[i+1] - m_akC[i]);
    }

    delete[] afDt;
    delete[] afD2t;
    delete[] akAlpha;
    delete[] afEll;
    delete[] afMu;
    delete[] akZ;
}
//----------------------------------------------------------------------------
void NaturalSpline2::CreateClosedSpline ()
{
    Real* afDt = new Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
        afDt[i] = m_afTime[i+1] - m_afTime[i];

    // TO DO.  Add ability to solve AX = B for B nxm (not just m=1) and
    // remove resetting of aafMat that occurs for each component of m_afC.

    // construct matrix of system
    Real** aafMat = LinearSystem::NewMatrix(m_iSegments+1);
    aafMat[0][0] = 1.0;
    aafMat[0][m_iSegments] = -1.0;
    for (i = 1; i <= m_iSegments-1; i++)
    {
        aafMat[i][i-1] = afDt[i-1];
        aafMat[i][i  ] = 2.0f*(afDt[i-1] + afDt[i]);
        aafMat[i][i+1] = afDt[i];
    }
    aafMat[m_iSegments][m_iSegments-1] = afDt[m_iSegments-1];
    aafMat[m_iSegments][0] = 2.0f*(afDt[m_iSegments-1] + afDt[0]);
    aafMat[m_iSegments][1] = afDt[0];

    // construct right-hand side of system
    m_akC = new Vector2[m_iSegments+1];
    m_akC[0] = Vector2::ZERO;
    Real fInv0, fInv1;
    for (i = 1; i <= m_iSegments-1; i++)
    {
        fInv0 = 1.0f/afDt[i];
        fInv1 = 1.0f/afDt[i-1];
        m_akC[i] = 3.0f*(fInv0*(m_akA[i+1] - m_akA[i]) - fInv1*(m_akA[i] -
            m_akA[i-1]));
    }
    fInv0 = 1.0f/afDt[0];
    fInv1 = 1.0f/afDt[m_iSegments-1];
    m_akC[m_iSegments] = 3.0f*(fInv0*(m_akA[1] - m_akA[0]) - fInv1*(m_akA[0] -
        m_akA[m_iSegments-1]));

    Real* afCx = LinearSystem::NewVector(m_iSegments+1);
    Real* afCy = LinearSystem::NewVector(m_iSegments+1);
    for (i = 0; i <= m_iSegments; i++)
    {
        afCx[i] = m_akC[i].x;
        afCy[i] = m_akC[i].y;
    }
    LinearSystem::Solve(m_iSegments+1,aafMat,afCx);

    // reset matrix for next system
    aafMat[0][0] = 1.0f;
    aafMat[0][m_iSegments] = -1.0f;
    for (i = 1; i <= m_iSegments-1; i++)
    {
        aafMat[i][i-1] = afDt[i-1];
        aafMat[i][i  ] = 2.0f*(afDt[i-1] + afDt[i]);
        aafMat[i][i+1] = afDt[i];
    }
    aafMat[m_iSegments][m_iSegments-1] = afDt[m_iSegments-1];
    aafMat[m_iSegments][0] = 2.0f*(afDt[m_iSegments-1] + afDt[0]);
    aafMat[m_iSegments][1] = afDt[0];

    LinearSystem::Solve(m_iSegments+1,aafMat,afCy);

    for (i = 0; i <= m_iSegments; i++)
    {
        m_akC[i].x = afCx[i];
        m_akC[i].y = afCy[i];
    }

    const Real fOneThird = 1.0f/3.0f;
    m_akB = new Vector2[m_iSegments];
    m_akD = new Vector2[m_iSegments];
    for (i = 0; i < m_iSegments; i++)
    {
        fInv0 = 1.0f/afDt[i];
        m_akB[i] = fInv0*(m_akA[i+1] - m_akA[i]) - fOneThird*(m_akC[i+1] +
            2.0f*m_akC[i])*afDt[i];
        m_akD[i] = fOneThird*fInv0*(m_akC[i+1] - m_akC[i]);
    }

    LinearSystem::DeleteMatrix(m_iSegments+1,aafMat);
    delete[] afDt;
    delete[] afCx;
    delete[] afCy;
}
//----------------------------------------------------------------------------
Vector2 NaturalSpline2::GetPosition (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2 kResult = m_akA[iKey] + fDt*(m_akB[iKey] + fDt*(
        m_akC[iKey] + fDt*m_akD[iKey]));

    return kResult;
}
//----------------------------------------------------------------------------
Vector2 NaturalSpline2::GetFirstDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2 kResult = m_akB[iKey] + fDt*(2.0f*m_akC[iKey] + 3.0f*fDt*
        m_akD[iKey]);

    return kResult;
}
//----------------------------------------------------------------------------
Vector2 NaturalSpline2::GetSecondDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2 kResult = 2.0f*m_akC[iKey] + 6.0f*fDt*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
Vector2 NaturalSpline2::GetThirdDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2 kResult = 6.0f*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
Real NaturalSpline2::GetSpeedKey (int iKey, Real fTime) const
{
    Vector2 kVelocity = m_akB[iKey] + fTime*(2.0f*m_akC[iKey] +
        3.0f*fTime*m_akD[iKey]);
    return kVelocity.Length();
}
//----------------------------------------------------------------------------
Real NaturalSpline2::GetLengthKey (int iKey, Real fT0, Real fT1) const
{
    _NaturalSpline2ThisPlusKey kData(this,iKey);
    return Integrate::RombergIntegral(fT0,fT1,GetSpeedWithData,(void*)&kData);
}
//----------------------------------------------------------------------------
Real NaturalSpline2::GetVariationKey (int iKey, Real fT0, Real fT1,
    const Vector2& rkA, const Vector2& rkB) const
{
    Polynomial kXPoly(3);
    kXPoly[0] = m_akA[iKey].x;
    kXPoly[1] = m_akB[iKey].x;
    kXPoly[2] = m_akC[iKey].x;
    kXPoly[3] = m_akD[iKey].x;

    Polynomial kYPoly(3);
    kYPoly[0] = m_akA[iKey].y;
    kYPoly[1] = m_akB[iKey].y;
    kYPoly[2] = m_akC[iKey].y;
    kYPoly[3] = m_akD[iKey].y;

    // construct line segment A + t*B
    Polynomial kLx(1), kLy(1);
    kLx[0] = rkA.x;
    kLx[1] = rkB.x;
    kLy[0] = rkA.y;
    kLy[1] = rkB.y;

    // compute |X(t) - L(t)|^2
    Polynomial kDx = kXPoly - kLx;
    Polynomial kDy = kYPoly - kLy;
    Polynomial kNormSqr = kDx*kDx + kDy*kDy;

    // compute indefinite integral of |X(t)-L(t)|^2
    Polynomial kIntegral(kNormSqr.GetDegree()+1);
    kIntegral[0] = 0.0f;
    for (int i = 1; i <= kIntegral.GetDegree(); i++)
        kIntegral[i] = kNormSqr[i-1]/i;

    // compute definite Integral(t0,t1,|X(t)-L(t)|^2)
    Real fResult = kIntegral(fT1) - kIntegral(fT0);
    return fResult;
}
//----------------------------------------------------------------------------


