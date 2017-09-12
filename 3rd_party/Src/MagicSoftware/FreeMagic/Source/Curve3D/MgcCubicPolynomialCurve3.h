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

#ifndef MGCCUBICPOLYNOMIALCURVE3_H
#define MGCCUBICPOLYNOMIALCURVE3_H

#include "MgcPolynomialCurve3.h"

namespace Mgc {


class MAGICFM CubicPolynomialCurve3 : public PolynomialCurve3
{
public:
    // Construction and destruction.  CubicPolynomialCurve3 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve3 (Polynomial* pkXPoly, Polynomial* pkYPoly,
        Polynomial* pkZPoly);

    virtual ~CubicPolynomialCurve3 ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector3* Vertices ();

    // tessellation by recursive subdivision
    void Tessellate (int iLevel);

protected:
    // precomputation
    class IntervalParameters
    {
    public:
        int m_iI0, m_iI1;
        Vector3 m_akXuu[2];
    };

    // subdivide curve into two halves
    void Subdivide (int iLevel, Real fDSqr, Vector3* akX,
        IntervalParameters& rkIP);

    // tessellation data
    int m_iVertexQuantity;
    Vector3* m_akVertex;
};

#include "MgcCubicPolynomialCurve3.inl"

} // namespace Mgc

#endif


