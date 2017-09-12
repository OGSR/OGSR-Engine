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

#ifndef MGCINTERP4DBSPLINE_H
#define MGCINTERP4DBSPLINE_H

#include "MgcInterpBSplineUniform.h"

namespace Mgc {

class Interp4DBSplineUniform : public InterpBSplineUniform
{
public:
    // Construction.  Interp4DBSplineUniform accepts responsibility for
    // deleting the input array afData.
    Interp4DBSplineUniform (int iDegree, const int* aiDim, Real* afData);

    int Index (int iX, int iY, int iZ, int iW) const;

    // spline evaluation for function interpolation (no derivatives)
    Real operator() (Real fX, Real fY, Real fZ, Real fW);
    virtual Real operator() (Real* afX);

    // spline evaluation, derivative counts given in iDx, iDy, iDz, iDw,
    // aiDx[]
    Real operator() (int iDx, int iDy, int iDz, int iDw, Real fX, Real fY,
        Real fZ, Real fW);
    virtual Real operator() (int* aiDx, Real* afX);

private:
    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

#include "MgcInterp4DBSplineUniform.inl"

} // namespace Mgc

#endif
