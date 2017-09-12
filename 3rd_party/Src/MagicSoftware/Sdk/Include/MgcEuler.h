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

#ifndef MGCEULER_H
#define MGCEULER_H

#include "MgcODE.h"

namespace Mgc {


class MAGICFM Euler : public ODE
{
public:
    // for dx/dt = F(t,x)
    Euler (int iDim, Real fStep, Function* aoF);
    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    // for dx/dt = A(x)
    Euler (int iDim, Real fStep, AutoFunction* aoA);
    virtual void Update (Real* afXIn, Real* afXOut);

    virtual void SetStepSize (Real fStep);
};

#include "MgcEuler.inl"

} // namespace Mgc

#endif

