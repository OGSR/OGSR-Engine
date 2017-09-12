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

#include "MgcEuler.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Euler::Euler (int iDim, Real fStep, Function* aoF)
    :
    ODE(iDim,fStep,aoF)
{
}
//----------------------------------------------------------------------------
Euler::Euler (int iDim, Real fStep, AutoFunction* aoA)
    :
    ODE(iDim,fStep,aoA)
{
}
//----------------------------------------------------------------------------
void Euler::Update (Real fTIn, Real* afXIn, Real& rfTOut, Real* afXOut)
{
    for (int i = 0; i < m_iDim; i++)
        afXOut[i] = afXIn[i] + m_fStep*m_aoF[i](fTIn,afXIn);

    rfTOut = fTIn + m_fStep;
}
//----------------------------------------------------------------------------
void Euler::Update (Real* afXIn, Real* afXOut)
{
    for (int i = 0; i < m_iDim; i++)
        afXOut[i] = afXIn[i] + m_fStep*m_aoA[i](afXIn);
}
//----------------------------------------------------------------------------

