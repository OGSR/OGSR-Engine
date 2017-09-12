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

#ifndef MGCINTERP1DAKIMA_H
#define MGCINTERP1DAKIMA_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Interp1DAkima
{
public:
    // abstract base class
    virtual ~Interp1DAkima ();

    class MAGICFM Polynomial
    {
    public:
        // P(x) = A[0]+A[1]*x+A[2]*x^2+A[3]*x^3
        Real& operator[] (int i) { return m_afA[i]; }

        Real operator() (Real fX) const
        {
            return m_afA[0]+fX*(m_afA[1]+fX*(m_afA[2]+fX*m_afA[3]));
        }

        Real operator() (unsigned int uiOrder, Real fX) const
        {
            switch ( uiOrder )
            {
            case 0:
                return m_afA[0]+fX*(m_afA[1]+fX*(m_afA[2]+fX*m_afA[3]));
            case 1:
                return m_afA[1]+fX*(2.0f*m_afA[2]+fX*3.0f*m_afA[3]);
            case 2:
                return 2.0f*m_afA[2]+fX*6.0f*m_afA[3];
            case 3:
                return 6.0f*m_afA[3];
            default:
                return 0.0f;
            }
        }

    protected:
        Real m_afA[4];
    };

    int GetQuantity () const;
    const Real* GetF () const;
    const Polynomial* GetPolynomials () const;
    const Polynomial& GetPolynomial (int i) const;

    virtual Real GetXMin () const = 0;
    virtual Real GetXMax () const = 0;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax.  If x is outside the
    // extremes, the function returns MAXREAL.  The first operator is for
    // function evaluation.  The second operator is for function or derivative
    // evaluations.  The uiOrder argument is the order of the derivative, zero
    // for the function itself.
    Real operator() (Real fX) const;
    Real operator() (int iOrder, Real fX) const;

protected:
    // Construction.  Interp1DAkima does not accept responsibility for
    // deleting the input array.  The application must do so.
    Interp1DAkima (int iQuantity, Real* afF);

    Real ComputeDerivative (Real* afSlope) const;
    virtual bool Lookup (Real fX, int& riIndex, Real& rfDX) const = 0;

    int m_iQuantity;
    Real* m_afF;
    Polynomial* m_akPoly;
};

#include "MgcInterp1DAkima.inl"

} // namespace Mgc

#endif


