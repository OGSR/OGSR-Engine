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

#ifndef MGCRK4_H
#define MGCRK4_H

#include "MgcODE.h"

namespace Mgc {


class MAGICFM RK4 : public ODE
{
public:
    virtual ~RK4 ();

    // for dx/dt = F(t,x)
    RK4 (int iDim, Real fStep, Function* aoF);
    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    // for dx/dt = A(x)
    RK4 (int iDim, Real fStep, AutoFunction* aoA);
    virtual void Update (Real* afXIn, Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    Real m_fHalfStep, m_fSixthStep;
    Real* m_afTemp1;
    Real* m_afTemp2;
    Real* m_afTemp3;
    Real* m_afTemp4;
    Real* m_afXTemp;

    static const Real ms_fOneSixth;
};

#include "MgcRK4.inl"

} // namespace Mgc

#endif

