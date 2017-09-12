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
#include "MgcPolynomial.h"
#include "MgcTCBSpline3.h"
using namespace Mgc;

// This used to be declared in the method GetLengthKey, but MipsPro 7.2.x on
// IRIX core dumps in that case.
class _TCBSpline3ThisPlusKey
{
public:
    _TCBSpline3ThisPlusKey (const TCBSpline3* pkThis, int iKey)
        : m_pkThis(pkThis), m_iKey(iKey) { /**/ }

    const TCBSpline3* m_pkThis;
    int m_iKey;
};

//----------------------------------------------------------------------------
TCBSpline3::TCBSpline3 (int iSegments, Real* afTime, Vector3* akPoint,
    Real* afTension, Real* afContinuity, Real* afBias)
    :
    MultipleCurve3(iSegments,afTime)
{
    // TO DO.  Add 'boundary type' just as in natural splines.
    assert( m_iSegments >= 3 );

    // all four of these arrays have m_iSegments+1 elements
    m_akPoint = akPoint;
    m_afTension = afTension;
    m_afContinuity = afContinuity;
    m_afBias = afBias;

    m_akA = new Vector3[m_iSegments];
    m_akB = new Vector3[m_iSegments];
    m_akC = new Vector3[m_iSegments];
    m_akD = new Vector3[m_iSegments];

    // For now, treat the first point as if it occurred twice.
    ComputePoly(0,0,1,2);

    for (int i = 1; i < m_iSegments-1; i++)
        ComputePoly(i-1,i,i+1,i+2);

    // For now, treat the last point as if it occurred twice.
    ComputePoly(m_iSegments-2,m_iSegments-1,m_iSegments,m_iSegments);

}
//----------------------------------------------------------------------------
TCBSpline3::~TCBSpline3 ()
{
    delete[] m_akPoint;
    delete[] m_afTension;
    delete[] m_afContinuity;
    delete[] m_afBias;
    delete[] m_akA;
    delete[] m_akB;
    delete[] m_akC;
    delete[] m_akD;
}
//----------------------------------------------------------------------------
void TCBSpline3::ComputePoly (int i0, int i1, int i2, int i3)
{
    Vector3 kDiff = m_akPoint[i2] - m_akPoint[i1];
    Real fDt = m_afTime[i2] - m_afTime[i1];

    // build multipliers at P1
    Real fOmt0 = 1.0f - m_afTension[i1];
    Real fOmc0 = 1.0f - m_afContinuity[i1];
    Real fOpc0 = 1.0f + m_afContinuity[i1];
    Real fOmb0 = 1.0f - m_afBias[i1];
    Real fOpb0 = 1.0f + m_afBias[i1];
    Real fAdj0 = 2.0f*fDt/(m_afTime[i2]-m_afTime[i0]);
    Real fOut0 = 0.5f*fAdj0*fOmt0*fOpc0*fOpb0;
    Real fOut1 = 0.5f*fAdj0*fOmt0*fOmc0*fOmb0;

    // build outgoing tangent at P1
    Vector3 kTOut = fOut1*kDiff + fOut0*(m_akPoint[i1] - m_akPoint[i0]);

    // build multipliers at point P2
    Real fOmt1 = 1.0f - m_afTension[i2];
    Real fOmc1 = 1.0f - m_afContinuity[i2];
    Real fOpc1 = 1.0f + m_afContinuity[i2];
    Real fOmb1 = 1.0f - m_afBias[i2];
    Real fOpb1 = 1.0f + m_afBias[i2];
    Real fAdj1 = 2.0f*fDt/(m_afTime[i3] - m_afTime[i1]);
    Real fIn0 = 0.5f*fAdj1*fOmt1*fOmc1*fOpb1;
    Real fIn1 = 0.5f*fAdj1*fOmt1*fOpc1*fOmb1;

    // build incoming tangent at P2
    Vector3 kTIn = fIn1*(m_akPoint[i3] - m_akPoint[i2]) + fIn0*kDiff;

    m_akA[i1] = m_akPoint[i1];
    m_akB[i1] = kTOut;
    m_akC[i1] = 3.0f*kDiff - 2.0f*kTOut - kTIn;
    m_akD[i1] = -2.0f*kDiff + kTOut + kTIn;
}
//----------------------------------------------------------------------------
Vector3 TCBSpline3::GetPosition (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector3 kResult = m_akA[iKey] + fDt*(m_akB[iKey] + fDt*(m_akC[iKey] +
        fDt*m_akD[iKey]));

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 TCBSpline3::GetFirstDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector3 kResult = m_akB[iKey] + fDt*(2.0f*m_akC[iKey] + 3.0f*fDt*
        m_akD[iKey]);

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 TCBSpline3::GetSecondDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector3 kResult = 2.0f*m_akC[iKey] + 6.0f*fDt*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
Vector3 TCBSpline3::GetThirdDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector3 kResult = 6.0f*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
Real TCBSpline3::GetSpeedKey (int iKey, Real fTime) const
{
    Vector3 kVelocity = m_akB[iKey] + fTime*(2.0f*m_akC[iKey] +
        3.0f*fTime*m_akD[iKey]);
    return kVelocity.Length();
}
//----------------------------------------------------------------------------
Real TCBSpline3::GetLengthKey (int iKey, Real fT0, Real fT1) const
{
    _TCBSpline3ThisPlusKey kData(this,iKey);
    return Integrate::RombergIntegral(fT0,fT1,GetSpeedWithData,(void*)&kData);
}
//----------------------------------------------------------------------------
Real TCBSpline3::GetVariationKey (int iKey, Real fT0, Real fT1,
    const Vector3& rkA, const Vector3& rkB) const
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

    Polynomial kZPoly(3);
    kZPoly[0] = m_akA[iKey].z;
    kZPoly[1] = m_akB[iKey].z;
    kZPoly[2] = m_akC[iKey].z;
    kZPoly[3] = m_akD[iKey].z;

    // construct line segment A + t*B
    Polynomial kLx(1), kLy(1), kLz(1);
    kLx[0] = rkA.x;
    kLx[1] = rkB.x;
    kLy[0] = rkA.y;
    kLy[1] = rkB.y;
    kLz[0] = rkA.z;
    kLz[1] = rkB.z;

    // compute |X(t) - L(t)|^2
    Polynomial kDx = kXPoly - kLx;
    Polynomial kDy = kYPoly - kLy;
    Polynomial kDz = kZPoly - kLz;
    Polynomial kNormSqr = kDx*kDx + kDy*kDy + kDz*kDz;

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


