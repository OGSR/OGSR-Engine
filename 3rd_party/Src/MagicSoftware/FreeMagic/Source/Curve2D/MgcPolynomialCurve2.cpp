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

#include "MgcPolynomialCurve2.h"
#include "MgcRTLib.h"
using namespace Mgc;

//----------------------------------------------------------------------------
PolynomialCurve2::PolynomialCurve2 (Polynomial* pkXPoly, Polynomial* pkYPoly)
    :
    SingleCurve2(0.0f,1.0f)
{
    assert( pkXPoly && pkYPoly );
    assert( pkXPoly->GetDegree() == pkYPoly->GetDegree() );

    m_pkXPoly = pkXPoly;
    m_pkYPoly = pkYPoly;
    m_kXDer1 = m_pkXPoly->GetDerivative();
    m_kYDer1 = m_pkYPoly->GetDerivative();
    m_kXDer2 = m_kXDer1.GetDerivative();
    m_kYDer2 = m_kYDer1.GetDerivative();
    m_kXDer3 = m_kXDer2.GetDerivative();
    m_kYDer3 = m_kYDer2.GetDerivative();
}
//----------------------------------------------------------------------------
PolynomialCurve2::~PolynomialCurve2 ()
{
    delete m_pkXPoly;
    delete m_pkYPoly;
}
//----------------------------------------------------------------------------
Vector2 PolynomialCurve2::GetPosition (Real fTime) const
{
    Vector2 kResult((*m_pkXPoly)(fTime),(*m_pkYPoly)(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector2 PolynomialCurve2::GetFirstDerivative (Real fTime) const
{
    Vector2 kResult(m_kXDer1(fTime),m_kYDer1(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector2 PolynomialCurve2::GetSecondDerivative (Real fTime) const
{
    Vector2 kResult(m_kXDer2(fTime),m_kYDer2(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Vector2 PolynomialCurve2::GetThirdDerivative (Real fTime) const
{
    Vector2 kResult(m_kXDer3(fTime),m_kYDer3(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
Real PolynomialCurve2::GetVariation (Real fT0, Real fT1,
    const Vector2* pkP0, const Vector2* pkP1) const
{
    Vector2 kP0, kP1;
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
    Vector2 kB = fInvDT*(*pkP1 - *pkP0);
    Vector2 kA = *pkP0 - fT0*kB;
    Polynomial kLx(1), kLy(1);
    kLx[0] = kA.x;
    kLx[1] = kB.x;
    kLy[0] = kA.y;
    kLy[1] = kB.y;

    // compute |X(t) - L(t)|^2
    Polynomial kDx = *m_pkXPoly - kLx;
    Polynomial kDy = *m_pkYPoly - kLy;
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


