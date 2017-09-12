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

#ifndef MGCINTERP2DBICUBIC_H
#define MGCINTERP2DBICUBIC_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Interp2DBicubic
{
public:
    // Construction and destruction.  Interp2DBicubic does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y)-values.  The
    // function values are assumed to be organized as f(x,y) = F[y][x].
    Interp2DBicubic (int iXBound, int iYBound, Real fXMin, Real fXSpacing,
        Real fYMin, Real fYSpacing, Real** aafF);

    int GetXBound () const;
    int GetYBound () const;
    int GetQuantity () const;
    Real** GetF () const;

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

    // convenience for creating/destroying arrays F[y][x]
    static void Allocate (int iXSize, int iYSize, Real**& raafArray);
    static void Deallocate (Real** aafArray);

protected:
    int m_iXBound, m_iYBound, m_iQuantity;
    Real m_fXMin, m_fXMax, m_fXSpacing, m_fInvXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing, m_fInvYSpacing;
    Real** m_aafF;
};

#include "MgcInterp2DBicubic.inl"

} // namespace Mgc

#endif


