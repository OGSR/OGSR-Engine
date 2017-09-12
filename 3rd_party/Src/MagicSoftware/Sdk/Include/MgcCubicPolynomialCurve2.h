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

#ifndef MGCCUBICPOLYNOMIALCURVE2_H
#define MGCCUBICPOLYNOMIALCURVE2_H

#include "MgcPolynomialCurve2.h"

namespace Mgc {


class MAGICFM CubicPolynomialCurve2 : public PolynomialCurve2
{
public:
    // Construction and destruction.  CubicPolynomialCurve2 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve2 (Polynomial* pkXPoly, Polynomial* pkYPoly);
    virtual ~CubicPolynomialCurve2 ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector2* Vertices ();

    // tessellation by recursive subdivision
    void Tessellate (int iLevel);

protected:
    // precomputation
    class MAGICFM IntervalParameters
    {
    public:
        int m_iI0, m_iI1;
        Vector2 m_akXuu[2];
    };

    // subdivide curve into two halves
    void Subdivide (int iLevel, Real fDSqr, Vector2* akX,
        IntervalParameters& rkIP);

    // tessellation data
    int m_iVertexQuantity;
    Vector2* m_akVertex;
};

#include "MgcCubicPolynomialCurve2.inl"

} // namespace Mgc

#endif


