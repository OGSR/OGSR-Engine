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

#ifndef MGCINTERP2DAKIMAUNIFORM_H
#define MGCINTERP2DAKIMAUNIFORM_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Interp2DAkimaUniform
{
public:
    // Construction and destruction.  Interp2DAkimaUniform does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y)-values.  The
    // function values are assumed to be organized as f(x,y) = F[y][x].
    Interp2DAkimaUniform (int iXBound, int iYBound, Real fXMin,
        Real fXSpacing, Real fYMin, Real fYSpacing, Real** aafF);

    ~Interp2DAkimaUniform ();

    class MAGICFM Polynomial
    {
    public:
        Polynomial ();

        // P(x,y) = (1,x,x^2,x^3)*A*(1,y,y^2,y^3).  The matrix term A[i][j]
        // corresponds to the polynomial term x^i y^j.
        Real& A (int iX, int iY)
        {
            return m_aafA[iX][iY];
        }

        Real operator() (Real fX, Real fY) const;
        Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const;

    protected:
        Real m_aafA[4][4];
    };

    int GetXBound () const;
    int GetYBound () const;
    int GetQuantity () const;
    Real** GetF () const;
    Polynomial** GetPolynomials () const;
    const Polynomial& GetPolynomial (int iX, int iY) const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax and ymin <= y <= ymax.
    // If (x,y) is outside the extremes, the function returns MAXREAL.  The
    // first operator is for function evaluation.  The second operator is for
    // function or derivative evaluations.  The uiXOrder argument is the order
    // of the x-derivative and the uiYOrder argument is the order of the
    // y-derivative.  Both orders are zero to get the function value itself.
    Real operator() (Real fX, Real fY) const;
    Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const;

    static void Allocate (int iXSize, int iYSize, Real**& raafArray);
    static void Deallocate (Real** aafArray);

protected:
    Real ComputeDerivative (Real* afSlope) const;
    void Construct (Polynomial& rkPoly, Real aafF[2][2],
        Real aafFX[2][2], Real aafFY[2][2], Real aafFXY[2][2]);

    bool XLookup (Real fX, int& riXIndex, Real& rfDX) const;
    bool YLookup (Real fY, int& riYIndex, Real& rfDY) const;

    int m_iXBound, m_iYBound, m_iQuantity;
    Real** m_aafF;
    Polynomial** m_aakPoly;
    Real m_fXMin, m_fXMax, m_fXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing;

    static void Allocate (int iXSize, int iYSize, Polynomial**& raakArray);
    static void Deallocate (Polynomial** aakArray);
};

#include "MgcInterp2DAkimaUniform.inl"

} // namespace Mgc

#endif


