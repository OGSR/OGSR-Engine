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

#include "MgcRK4.h"
using namespace Mgc;

const Real RK4::ms_fOneSixth = 1.0f/6.0f;

//----------------------------------------------------------------------------
RK4::RK4 (int iDim, Real fStep, Function* aoF)
    :
    ODE(iDim,fStep,aoF)
{
    m_fHalfStep = 0.5f*fStep;
    m_fSixthStep = ms_fOneSixth*fStep;
    m_afTemp1 = new Real[iDim];
    m_afTemp2 = new Real[iDim];
    m_afTemp3 = new Real[iDim];
    m_afTemp4 = new Real[iDim];
    m_afXTemp = new Real[iDim];
}
//----------------------------------------------------------------------------
RK4::RK4 (int iDim, Real fStep, AutoFunction* aoA)
    :
    ODE(iDim,fStep,aoA)
{
    m_fHalfStep = 0.5f*fStep;
    m_fSixthStep = ms_fOneSixth*fStep;
    m_afTemp1 = new Real[iDim];
    m_afTemp2 = new Real[iDim];
    m_afTemp3 = new Real[iDim];
    m_afTemp4 = new Real[iDim];
    m_afXTemp = new Real[iDim];
}
//----------------------------------------------------------------------------
RK4::~RK4 ()
{
    delete[] m_afTemp1;
    delete[] m_afTemp2;
    delete[] m_afTemp3;
    delete[] m_afTemp4;
    delete[] m_afXTemp;
}
//----------------------------------------------------------------------------
void RK4::Update (Real fTIn, Real* afXIn, Real& rfTOut, Real* afXOut)
{
    // first step
    int i;
    for (i = 0; i < m_iDim; i++)
        m_afTemp1[i] = m_aoF[i](fTIn,afXIn);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp1[i];

    // second step
    Real fHalfT = fTIn + m_fHalfStep;
    for (i = 0; i < m_iDim; i++)
        m_afTemp2[i] = m_aoF[i](fHalfT,m_afXTemp);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp2[i];

    // third step
    for (i = 0; i < m_iDim; i++)
        m_afTemp3[i] = m_aoF[i](fHalfT,m_afXTemp);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fStep*m_afTemp3[i];

    // fourth step
    rfTOut = fTIn + m_fStep;
    for (i = 0; i < m_iDim; i++)
        m_afTemp4[i] = m_aoF[i](rfTOut,m_afXTemp);
    for (i = 0; i < m_iDim; i++)
    {
        afXOut[i] = afXIn[i] + m_fSixthStep*(m_afTemp1[i] +
            2.0f*(m_afTemp2[i] + m_afTemp3[i]) + m_afTemp4[i]);
    }
}
//----------------------------------------------------------------------------
void RK4::Update (Real* afXIn, Real* afXOut)
{
    // first step
    int i;
    for (i = 0; i < m_iDim; i++)
        m_afTemp1[i] = m_aoA[i](afXIn);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp1[i];

    // second step
    for (i = 0; i < m_iDim; i++)
        m_afTemp2[i] = m_aoA[i](m_afXTemp);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp2[i];

    // third step
    for (i = 0; i < m_iDim; i++)
        m_afTemp3[i] = m_aoA[i](m_afXTemp);
    for (i = 0; i < m_iDim; i++)
        m_afXTemp[i] = afXIn[i] + m_fStep*m_afTemp3[i];

    // fourth step
    for (i = 0; i < m_iDim; i++)
        m_afTemp4[i] = m_aoA[i](m_afXTemp);
    for (i = 0; i < m_iDim; i++)
    {
        afXOut[i] = afXIn[i] + m_fSixthStep*(m_afTemp1[i] +
            2.0f*(m_afTemp2[i] + m_afTemp3[i]) + m_afTemp4[i]);
    }
}
//----------------------------------------------------------------------------

