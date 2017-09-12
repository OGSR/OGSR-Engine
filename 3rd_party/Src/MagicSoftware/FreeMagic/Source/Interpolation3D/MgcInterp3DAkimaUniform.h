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

#ifndef MGCINTERP3DAKIMAUNIFORM_H
#define MGCINTERP3DAKIMAUNIFORM_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Interp3DAkimaUniform
{
public:
    // Construction and destruction.  Interp3DAkimaUniform does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y,z)-values.  The
    // function values are assumed to be organized as f(x,y,z) = F[z][y][x].
    Interp3DAkimaUniform (int iXBound, int iYBound, int iZBound, Real fXMin,
        Real fXSpacing, Real fYMin, Real fYSpacing, Real fZMin,
        Real fZSpacing, Real*** aaafF);

    ~Interp3DAkimaUniform ();

    class MAGICFM Polynomial
    {
    public:
        Polynomial ();

        // P(x,y,z) = sum_{i=0}^3 sum_{j=0}^3 sum_{k=0}^3 a_{ijk} x^i y^j z^k
        Real& A (int iX, int iY, int iZ)
        {
            return m_aaafA[iX][iY][iZ];
        }

        Real operator() (Real fX, Real fY, Real fZ) const;
        Real operator() (int iXOrder, int iYOrder, int iZOrder, Real fX,
            Real fY, Real fZ) const;

    protected:
        Real m_aaafA[4][4][4];
    };

    int GetXBound () const;
    int GetYBound () const;
    int GetZBound () const;
    int GetQuantity () const;
    Real*** GetF () const;
    Polynomial*** GetPolynomials () const;
    const Polynomial& GetPolynomial (int iX, int iY, int iZ) const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;
    Real GetZMin () const;
    Real GetZMax () const;
    Real GetZSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax, ymin <= y <= ymax,
    // and zmin <= z <= zmax.  If (x,y,z) is outside the extremes, the
    // function returns MAXREAL.  The first operator is for function
    // evaluation.  The second operator is for function or derivative
    // evaluations.  The uiXOrder argument is the order of the x-derivative,
    // the uiYOrder argument is the order of the y-derivative, and the
    // uiZOrder argument is the order of the z-derivative.  All orders are
    // zero to get the function value itself.
    Real operator() (Real fX, Real fY, Real fZ) const;
    Real operator() (int iXOrder, int iYOrder, int iZOrder, Real fX,
        Real fY, Real fZ) const;

    static void Allocate (int iXSize, int iYSize, int iZSize,
        Real***& raaafArray);
    static void Deallocate (int iYSize, int iZSize, Real*** aaafArray);

protected:
    Real ComputeDerivative (Real* afSlope) const;
    void Construct (Polynomial& rkPoly, Real aaafF[2][2][2],
        Real aaafFX[2][2][2], Real aaafFY[2][2][2],
        Real aaafFZ[2][2][2], Real aaafFXY[2][2][2],
        Real aaafFXZ[2][2][2], Real aaafFYZ[2][2][2],
        Real aaafFXYZ[2][2][2]);

    bool XLookup (Real fX, int& riXIndex, Real& rfDX) const;
    bool YLookup (Real fY, int& riYIndex, Real& rfDY) const;
    bool ZLookup (Real fZ, int& riZIndex, Real& rfDZ) const;

    int m_iXBound, m_iYBound, m_iZBound, m_iQuantity;
    Real*** m_aaafF;
    Polynomial*** m_aaakPoly;
    Real m_fXMin, m_fXMax, m_fXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing;
    Real m_fZMin, m_fZMax, m_fZSpacing;

    static void Allocate (int iXSize, int iYSize, int iZSize,
        Polynomial***& raaakArray);
    static void Deallocate (int iYSize, int iZSize, Polynomial*** aaakArray);
};

#include "MgcInterp3DAkimaUniform.inl"

} // namespace Mgc;

#endif


