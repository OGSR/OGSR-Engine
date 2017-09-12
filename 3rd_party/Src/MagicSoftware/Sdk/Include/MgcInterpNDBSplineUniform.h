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

#ifndef MGCINTERPNDBSPLINEUNIFORM_H
#define MGCINTERPNDBSPLINEUNIFORM_H

#include "MgcInterpBSplineUniform.h"

namespace Mgc {

class InterpNDBSplineUniform : public InterpBSplineUniform
{
public:
    // Construction and destruction.  InterpNDBSplineUniform accepts
    // responsibility for deleting the input array afData.  The input array
    // aiDim is copied.
    InterpNDBSplineUniform (int iDims, int iDegree, const int* aiDim,
        Real* afData);
    virtual ~InterpNDBSplineUniform ();

    int Index (int* aiI) const;

    // spline evaluation for function interpolation (no derivatives)
    virtual Real operator() (Real* afX);

    // spline evaluation, derivative counts given in aiDx[]
    virtual Real operator() (int* aiDx, Real* afX);

private:
    int* m_aiEvI;
    int* m_aiCiLoop;
    int* m_aiCiDelta;
    int* m_aiOpI;
    int* m_aiOpJ;
    int* m_aiOpDelta;

    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

} // namespace Mgc

#endif
