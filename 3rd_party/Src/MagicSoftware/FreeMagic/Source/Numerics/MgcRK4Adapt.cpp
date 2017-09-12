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

#include "MgcRK4Adapt.h"
using namespace Mgc;

const Real RK4Adapt::ms_fOneSixth = 1.0f/6.0f;
const Real RK4Adapt::ms_fOneTwelfth = 1.0f/12.0f;
const Real RK4Adapt::ms_fOneFifteenth = 1.0f/15.0f;

//----------------------------------------------------------------------------
RK4Adapt::RK4Adapt (int iDim, Real fStep, Function* aoF)
    :
    ODE(iDim,fStep,aoF)
{
    m_fStepUsed = 0.0f;
    m_iPasses = 0;
    m_fSafety = 0.9f;
    m_fEpsilon = 0.01f;
    m_fCoeff = Math::Pow(0.25f*m_fSafety,0.2f);

    m_afTemp1 = new Real[m_iDim];
    m_afTemp2 = new Real[m_iDim];
    m_afTemp3 = new Real[m_iDim];
    m_afTemp4 = new Real[m_iDim];
    m_afSave1 = new Real[m_iDim];
    m_afXTemp  = new Real[m_iDim];
    m_afXInter = new Real[m_iDim];
    m_afXHalf  = new Real[m_iDim];
    m_afXFull  = new Real[m_iDim];
}
//----------------------------------------------------------------------------
RK4Adapt::RK4Adapt (int iDim, Real fStep, AutoFunction* aoA)
    :
    ODE(iDim,fStep,aoA)
{
    m_fStepUsed = 0.0f;
    m_iPasses = 0;
    m_fSafety = 0.9f;
    m_fEpsilon = 0.01f;
    m_fCoeff = Math::Pow(0.25f*m_fSafety,0.2f);

    m_afTemp1 = new Real[m_iDim];
    m_afTemp2 = new Real[m_iDim];
    m_afTemp3 = new Real[m_iDim];
    m_afTemp4 = new Real[m_iDim];
    m_afSave1 = new Real[m_iDim];
    m_afXTemp  = new Real[m_iDim];
    m_afXInter = new Real[m_iDim];
    m_afXHalf  = new Real[m_iDim];
    m_afXFull  = new Real[m_iDim];
}
//----------------------------------------------------------------------------
RK4Adapt::~RK4Adapt ()
{
    delete[] m_afTemp1;
    delete[] m_afTemp2;
    delete[] m_afTemp3;
    delete[] m_afTemp4;
    delete[] m_afSave1;
    delete[] m_afXTemp;
    delete[] m_afXInter;
    delete[] m_afXHalf;
    delete[] m_afXFull;
}
//----------------------------------------------------------------------------
void RK4Adapt::SetSafety (Real fSafety)
{
    m_fSafety = fSafety;
    m_fCoeff = Math::Pow(0.25f*m_fSafety,0.2f);
}
//----------------------------------------------------------------------------
void RK4Adapt::Update (Real fTIn, Real* afXIn, Real& rfTOut, Real* afXOut)
{
    m_iPasses = 1;
    while ( true )
    {
        Real fH2 = 0.5f*m_fStep;
        Real fH4 = 0.25f*m_fStep;
        Real fH6 = ms_fOneSixth*m_fStep;
        Real fH12 = ms_fOneTwelfth*m_fStep;
        Real fT2 = fTIn + fH2;
        Real fT4 = fTIn + fH4;
        Real fT2pH4 = fT2 + fH4;
        rfTOut = fTIn + m_fStep;

        // take a half-step
        int i;
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afTemp1[i] = m_afSave1[i] = m_aoF[i](fTIn,afXIn);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH4*m_afTemp1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoF[i](fT4,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH4*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoF[i](fT4,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH2*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoF[i](fT2,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXInter[i] = afXIn[i]+fH12*(m_afTemp1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // take another half-step
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afTemp1[i] = m_aoF[i](fT2,m_afXInter);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH4*m_afTemp1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoF[i](fT2pH4,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH4*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoF[i](fT2pH4,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH2*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoF[i](rfTOut,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXHalf[i] = m_afXInter[i]+fH12*(m_afTemp1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // take a full-step
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afXTemp[i] = afXIn[i]+fH2*m_afSave1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoF[i](fT2,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH2*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoF[i](fT2,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+m_fStep*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoF[i](rfTOut,m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXFull[i] = afXIn[i]+fH6*(m_afSave1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // compute error term
        Real fMaxRatio = 0.0f;
        for (i = 0; i < m_iDim; i++)
        {
            m_afXTemp[i] = m_afXHalf[i]-m_afXFull[i];
            Real fRatio = Math::FAbs(m_afXTemp[i]/(m_fStep*m_afSave1[i] +
                1e-16f));
            if ( fMaxRatio < fRatio )
                fMaxRatio = fRatio;
        }
        fMaxRatio /= m_fEpsilon;

        if ( fMaxRatio <= 1.0f )
        {
            // increase the step size
            m_fStepUsed = m_fStep;
            if ( fMaxRatio > m_fCoeff )
                m_fStep *= m_fSafety/Math::Pow(fMaxRatio,0.2f);
            else
                m_fStep *= 4.0f;
            break;
        }
        // else decrease the step size, recompute solution with it
        m_iPasses++;
        m_fStep *= m_fSafety/Math::Pow(fMaxRatio,0.25f);
        if ( m_fStep == 0.0f )  // step size became too small
            return;
    }

    // generate output
    for (int i = 0; i < m_iDim; i++)
        afXOut[i] = m_afXHalf[i] + ms_fOneFifteenth*m_afXTemp[i];
}
//----------------------------------------------------------------------------
void RK4Adapt::Update (Real* afXIn, Real* afXOut)
{
    m_iPasses = 1;
    while ( true )
    {
        Real fH2 = 0.5f*m_fStep;
        Real fH4 = 0.25f*m_fStep;
        Real fH6 = ms_fOneSixth*m_fStep;
        Real fH12 = ms_fOneTwelfth*m_fStep;

        // take a half-step
        int i;
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afTemp1[i] = m_afSave1[i] = m_aoA[i](afXIn);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH4*m_afTemp1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH4*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH2*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXInter[i] = afXIn[i]+fH12*(m_afTemp1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // Take another half-step
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afTemp1[i] = m_aoA[i](m_afXInter);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH4*m_afTemp1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH4*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = m_afXInter[i]+fH2*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXHalf[i] = m_afXInter[i]+fH12*(m_afTemp1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // Take a full-step
        for (i = 0; i < m_iDim; i++)  // RK4 first step
            m_afXTemp[i] = afXIn[i]+fH2*m_afSave1[i];
        for (i = 0; i < m_iDim; i++)  // RK4 second step
            m_afTemp2[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+fH2*m_afTemp2[i];
        for (i = 0; i < m_iDim; i++)  // RK4 third step
            m_afTemp3[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
            m_afXTemp[i] = afXIn[i]+m_fStep*m_afTemp3[i];
        for (i = 0; i < m_iDim; i++)  // RK4 fourth step
            m_afTemp4[i] = m_aoA[i](m_afXTemp);
        for (i = 0; i < m_iDim; i++)
        {
            m_afXFull[i] = afXIn[i]+fH6*(m_afSave1[i]+
                2.0f*(m_afTemp2[i]+m_afTemp3[i])+m_afTemp4[i]);
        }

        // compute error term
        Real fMaxRatio = 0.0f; 
        for (i = 0; i < m_iDim; i++)
        {
            m_afXTemp[i] = m_afXHalf[i]-m_afXFull[i];
            Real fRatio = Math::FAbs(m_afXTemp[i]/(m_fStep*m_afSave1[i] +
                1e-16f));
            if ( fMaxRatio < fRatio )
                fMaxRatio = fRatio;
        }
        fMaxRatio /= m_fEpsilon;

        if ( fMaxRatio <= 1.0f )
        {
            // increase the step size
            m_fStepUsed = m_fStep;
            if ( fMaxRatio > m_fCoeff )
                m_fStep *= m_fSafety/Math::Pow(fMaxRatio,0.2f);
            else
                m_fStep *= 4.0f;
            break;
        }
        // else decrease the step size, recompute solution with it
        m_iPasses++;
        m_fStep *= m_fSafety/Math::Pow(fMaxRatio,0.25f);
        if ( m_fStep == 0.0f )  // step size became too small
            return;
    }

    // generate output
    for (int i = 0; i < m_iDim; i++)
        afXOut[i] = m_afXHalf[i] + ms_fOneFifteenth*m_afXTemp[i];
}
//----------------------------------------------------------------------------
void RK4Adapt::SetStepSize (Real fStep)
{
    m_fStep = fStep;
    m_fStepUsed = 0.0f;
}
//----------------------------------------------------------------------------

