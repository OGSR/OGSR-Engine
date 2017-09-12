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

#ifndef MGCRK4ADAPT_H
#define MGCRK4ADAPT_H

#include "MgcODE.h"

namespace Mgc {


class MAGICFM RK4Adapt : public ODE
{
public:
    virtual ~RK4Adapt ();

    // for dx/dt = F(t,x)
    RK4Adapt (int iDim, Real fStep, Function* aoF);
    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    // for dx/dt = A(x)
    RK4Adapt (int iDim, Real fStep, AutoFunction* aoA);
    virtual void Update (Real* afXIn, Real* afXOut);

    virtual void SetStepSize (Real fStep);

    void SetSafety (Real fSafety);
    Real GetSafety () const;
    void SetEpsilon (Real fEpsilon);
    Real GetEpsilon () const;
    Real GetStepUsed () const;
    int GetPasses () const;

protected:
    int m_iPasses;
    Real m_fStepUsed, m_fSafety, m_fEpsilon, m_fCoeff;
    Real* m_afTemp1;
    Real* m_afTemp2;
    Real* m_afTemp3;
    Real* m_afTemp4;
    Real* m_afSave1;
    Real* m_afXTemp;
    Real* m_afXInter;
    Real* m_afXHalf;
    Real* m_afXFull;

    static const Real ms_fOneSixth;
    static const Real ms_fOneTwelfth;
    static const Real ms_fOneFifteenth;
};

#include "MgcRK4Adapt.inl"

} // namespace Mgc

#endif

