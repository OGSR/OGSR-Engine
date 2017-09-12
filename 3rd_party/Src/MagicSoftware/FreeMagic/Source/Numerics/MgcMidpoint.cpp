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

#include "MgcMidpoint.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Midpoint::Midpoint (int iDim, Real fStep, Function* aoF)
    :
    ODE(iDim,fStep,aoF)
{
    m_fHalfStep = 0.5f*fStep;
    m_afXTemp = new Real[iDim];
}
//----------------------------------------------------------------------------
Midpoint::Midpoint (int iDim, Real fStep, AutoFunction* aoA)
    :
    ODE(iDim,fStep,aoA)
{
    m_fHalfStep = 0.5f*fStep;
    m_afXTemp = new Real[iDim];
}
//----------------------------------------------------------------------------
Midpoint::~Midpoint ()
{
    delete[] m_afXTemp;
}
//----------------------------------------------------------------------------
void Midpoint::Update (Real fTIn, Real* afXIn, Real& rfTOut, Real* afXOut)
{
    // first step
    int i;
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_aoF[i](fTIn,afXIn);

    // second step
    Real fHalfT = fTIn + m_fHalfStep;
    for (i = 0; i < m_iDim; i++)
        afXOut[i] = afXIn[i] + m_fStep*m_aoF[i](fHalfT,m_afXTemp);

    rfTOut = fTIn + m_fStep;
}
//----------------------------------------------------------------------------
void Midpoint::Update (Real* afXIn, Real* afXOut)
{
    // first step
    int i;
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_aoA[i](afXIn);

    // second step
    for (i = 0; i < m_iDim; i++)
        afXOut[i] = afXIn[i] + m_fStep*m_aoA[i](m_afXTemp);
}
//----------------------------------------------------------------------------

