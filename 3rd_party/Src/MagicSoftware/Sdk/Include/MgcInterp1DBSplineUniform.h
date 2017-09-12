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

#ifndef MGCINTERP1DBSPLINEUNIFORM_H
#define MGCINTERP1DBSPLINEUNIFORM_H

#include "MgcInterpBSplineUniform.h"

namespace Mgc {

class Interp1DBSplineUniform : public InterpBSplineUniform
{
public:
    // Construction.  Interp1DBSplineUniform accepts responsibility for
    // deleting the input array afData.
    Interp1DBSplineUniform (int iDegree, int iDim, Real* afData);

    // spline evaluation for function interpolation (no derivatives)
    Real operator() (Real fX);
    virtual Real operator() (Real* afX);

    // spline evaluation, derivative count given in iDx and aiDx[]
    Real operator() (int iDx, Real fX);
    virtual Real operator() (int* aiDx, Real* afX);

private:
    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

#include "MgcInterp1DBSplineUniform.inl"

} // namespace Mgc

#endif
