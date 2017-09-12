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

#ifndef MGCINTERP2DTHINPLATESPLINE_H
#define MGCINTERP2DTHINPLATESPLINE_H

// WARNING.  This code maps the inputs (x,y) to the unit square.  The thin
// plate spline function evaluation maps the input to the unit square and
// performs the interpolation in that space.  The idea is to keep the floating
// point numbers to order 1 for numerical stability of the algorithm. Some
// folks are not excited about this preprocessing step as it is not part of
// the classical thin plate spline algorithm. It is easy enough to remove the
// remappings from the code.

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Interp2DThinPlateSpline
{
public:
    // Construction and destruction.  Data points are of form (x,y,f(x,y)).
    // The smoothing parameter must be nonnegative.  The caller is responsible
    // for deleting the input arrays.
    Interp2DThinPlateSpline (int iQuantity, Real* afX, Real* afY,
        Real* afF, Real fSmooth = 0.0f);

    ~Interp2DThinPlateSpline ();

    // Check this after the constructor call to see if the thin plate spline
    // coefficients were successfully computed.  If so, then calls to
    // operator()(Real,Real,Real) will work properly.
    bool IsInitialized () const;

    // Evaluate the thin plate spline interpolator.
    Real operator() (Real fX, Real fY);

protected:
    static Real Kernel (Real fT);

    bool m_bInitialized;
    int m_iQuantity;

    // input data mapped to unit cube
    Real* m_afX;
    Real* m_afY;

    // thin plate spline coefficients
    Real* m_afA;
    Real m_afB[3];

    // extent of input data
    Real m_fXMin, m_fXMax, m_fXInvRange;
    Real m_fYMin, m_fYMax, m_fYInvRange;
};

} // namespace Mgc

#endif


