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

#ifndef MGCPOLYNOMIALCURVE2_H
#define MGCPOLYNOMIALCURVE2_H

#include "MgcPolynomial.h"
#include "MgcSingleCurve2.h"

namespace Mgc {


class MAGICFM PolynomialCurve2 : public SingleCurve2
{
public:
    // Construction and destruction.  PolynomialCurve2 accepts responsibility
    // for deleting the input polynomials.
    PolynomialCurve2 (Polynomial* pkXPoly, Polynomial* pkYPoly);
    virtual ~PolynomialCurve2 ();

    int GetDegree () const;
    const Polynomial* GetXPolynomial () const;
    const Polynomial* GetYPolynomial () const;

    virtual Vector2 GetPosition (Real fTime) const;
    virtual Vector2 GetFirstDerivative (Real fTime) const;
    virtual Vector2 GetSecondDerivative (Real fTime) const;
    virtual Vector2 GetThirdDerivative (Real fTime) const;

    virtual Real GetVariation (Real fT0, Real fT1,
        const Vector2* pkP0 = 0, const Vector2* pkP1 = 0) const;

protected:
    Polynomial* m_pkXPoly;
    Polynomial* m_pkYPoly;
    Polynomial m_kXDer1, m_kYDer1;
    Polynomial m_kXDer2, m_kYDer2;
    Polynomial m_kXDer3, m_kYDer3;
};

#include "MgcPolynomialCurve2.inl"

} // namespace Mgc

#endif


