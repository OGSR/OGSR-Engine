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

#include "MgcPolynomialCurve3.h"
using namespace Mgc;

//----------------------------------------------------------------------------
PolynomialCurve3::PolynomialCurve3 (Polynomial* pkXPoly, Polynomial* pkYPoly,
    Polynomial* pkZPoly)
    :
    SingleCurve3(0.0f,1.0f)
{
    assert( pkXPoly && pkYPoly && pkZPoly );
    assert( pkXPoly->GetDegree() == pkYPoly->GetDegree() );
    assert( pkXPoly->GetDegree() == pkZPoly->GetDegree() );

    m_pkXPoly = pkXPoly;
    m_pkYPoly = pkYPoly;
    m_pkZPoly = pkZPoly;
    m_kXDer1 = m_pkXPoly->GetDerivative();
    m_kYDer1 = m_pkYPoly->GetDerivative();
    m_kZDer1 = m_pkZPoly->GetDerivative();
    m_kXDer2 = m_kXDer1.GetDerivative();
    m_kYDer2 = m_kYDer1.GetDerivative();
    m_kZDer2 = m_kZDer1.GetDerivative();
    m_kXDer3 = m_kXDer2.GetDerivative();
    m_kYDer3 = m_kYDer2.GetDerivative();
    m_kZDer3 = m_kZDer2.GetDerivative();
}
//----------------------------------------------------------------------------
PolynomialCurve3::~PolynomialCurve3 ()
{
    delete m_pkXPoly;
    delete m_pkYPoly;
    delete m_pkZPoly;
}
//----------------------------------------------------------------------------
Vector3 PolynomialCurve3::GetPosition (Real fTime) const
{
    Vector3 kResult((*m_pkXPoly)(fTime),(*m_pkYPoly)(fTime),
        (*m_pkZPoly)(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 PolynomialCurve3::GetFirstDerivative (Real fTime) const
{
    Vector3 kResult(m_kXDer1(fTime),m_kYDer1(fTime),m_kZDer1(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 PolynomialCurve3::GetSecondDerivative (Real fTime) const
{
    Vector3 kResult(m_kXDer2(fTime),m_kYDer2(fTime),m_kZDer2(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 PolynomialCurve3::GetThirdDerivative (Real fTime) const
{
    Vector3 kResult(m_kXDer3(fTime),m_kYDer3(fTime),m_kZDer3(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Real PolynomialCurve3::GetVariation (Real fT0, Real fT1,
    const Vector3* pkP0, const Vector3* pkP1) const
{
    Vector3 kP0, kP1;
    if ( !pkP0 )
    {
        kP0 = GetPosition(fT0);
        pkP0 = &kP0;
    }
    if ( !pkP1 )
    {
        kP1 = GetPosition(fT1);
        pkP1 = &kP1;
    }

    // construct line segment A + t*B
    Real fInvDT = 1.0f/(fT1 - fT0);
    Vector3 kB = fInvDT*(*pkP1 - *pkP0);
    Vector3 kA = *pkP0 - fT0*kB;
    Polynomial kLx(1), kLy(1), kLz(1);
    kLx[0] = kA.x;
    kLx[1] = kB.x;
    kLy[0] = kA.y;
    kLy[1] = kB.y;
    kLz[0] = kA.z;
    kLz[1] = kB.z;

    // compute |X(t) - L(t)|^2
    Polynomial kDx = *m_pkXPoly - kLx;
    Polynomial kDy = *m_pkYPoly - kLy;
    Polynomial kDz = *m_pkZPoly - kLz;
    Polynomial kNormSqr = kDx*kDx + kDy*kDy + kDz*kDz;

    // compute indefinite integral of |X(t)-L(t)|^2
    Polynomial kIntegral(kNormSqr.GetDegree()+1);
    kIntegral[0] = 0.0f;
    for (int i = 1; i <= kIntegral.GetDegree(); i++)
        kIntegral[i] = kNormSqr[i-1]/i;

    // return definite Integral(t0,t1,|X(t)-L(t)|^2)
    Real fResult = kIntegral(fT1) - kIntegral(fT0);
    return fResult;
}
//----------------------------------------------------------------------------


