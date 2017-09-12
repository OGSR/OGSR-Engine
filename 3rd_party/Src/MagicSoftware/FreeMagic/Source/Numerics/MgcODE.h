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

#ifndef MGCODE_H
#define MGCODE_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM ODE 
{
public:
    virtual ~ODE ();

    // for dx/dt = F(t,x)
    typedef Real (*Function)(Real,Real*);
    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut) = 0;

    // for dx/dt = A(x)
    typedef Real (*AutoFunction)(Real*);
    virtual void Update (Real* afXIn, Real* afXOut) = 0;

    virtual void SetStepSize (Real fStep) = 0;
    Real GetStepSize () const;

protected:
    ODE (int iDim, Real fStep, Function* aoF);
    ODE (int iDim, Real fStep, AutoFunction* aoA);

    int m_iDim;
    Real m_fStep;
    Function* m_aoF;
    AutoFunction* m_aoA;
};

#include "MgcODE.inl"

} // namespace Mgc

#endif

