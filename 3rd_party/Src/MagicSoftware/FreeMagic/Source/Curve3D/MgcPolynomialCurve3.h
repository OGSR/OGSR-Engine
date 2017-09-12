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

#ifndef MGCPOLYNOMIALCURVE3_H
#define MGCPOLYNOMIALCURVE3_H

#include "MgcPolynomial.h"
#include "MgcSingleCurve3.h"

namespace Mgc {


class MAGICFM PolynomialCurve3 : public SingleCurve3
{
public:
    // Construction and destruction.  PolynomialCurve3 accepts responsibility
    // for deleting the input polynomials.
    PolynomialCurve3 (Polynomial* pkXPoly, Polynomial* pkYPoly,
        Polynomial* pkZPoly);

    virtual ~PolynomialCurve3 ();

    int GetDegree () const;
    const Polynomial* GetXPolynomial () const;
    const Polynomial* GetYPolynomial () const;
    const Polynomial* GetZPolynomial () const;

    virtual Vector3 GetPosition (Real fTime) const;
    virtual Vector3 GetFirstDerivative (Real fTime) const;
    virtual Vector3 GetSecondDerivative (Real fTime) const;
    virtual Vector3 GetThirdDerivative (Real fTime) const;

    virtual Real GetVariation (Real fT0, Real fT1, const Vector3* pkP0 = 0,
        const Vector3* pkP1 = 0) const;

protected:
    Polynomial* m_pkXPoly;
    Polynomial* m_pkYPoly;
    Polynomial* m_pkZPoly;
    Polynomial m_kXDer1, m_kYDer1, m_kZDer1;
    Polynomial m_kXDer2, m_kYDer2, m_kZDer2;
    Polynomial m_kXDer3, m_kYDer3, m_kZDer3;
};

#include "MgcPolynomialCurve3.inl"

} // namespace Mgc

#endif


