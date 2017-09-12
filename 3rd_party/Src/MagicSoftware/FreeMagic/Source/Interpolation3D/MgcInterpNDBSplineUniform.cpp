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

#include "MgcInterpNDBSplineUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
InterpNDBSplineUniform::InterpNDBSplineUniform (int iDims, int iDegree,
    const int* aiDim, Real* afData)
    :
    InterpBSplineUniform(iDims,iDegree,aiDim,afData)
{
    m_aiEvI = new int[m_iDims];
    m_aiCiLoop = new int[m_iDims];
    m_aiCiDelta = new int[m_iDims-1];
    m_aiOpI = new int[m_iDims];
    m_aiOpJ = new int[m_iDims];
    m_aiOpDelta = new int[m_iDims-1];

    int iTemp = 1;
    for (int iDim = 0; iDim < m_iDims-1; iDim++)
    {
        m_aiCiDelta[iDim] = iTemp*(m_aiDim[iDim]-m_iDp1);
        iTemp *= m_aiDim[iDim];
    }
}
//----------------------------------------------------------------------------
InterpNDBSplineUniform::~InterpNDBSplineUniform ()
{
    delete[] m_aiEvI;
    delete[] m_aiCiLoop;
    delete[] m_aiOpI;
    delete[] m_aiOpJ;
    delete[] m_aiOpDelta;
    delete[] m_aiCiDelta;
}
//----------------------------------------------------------------------------
int InterpNDBSplineUniform::Index (int* aiI) const
{
    int iIndex = aiI[m_iDims-1];
    for (int iDim = m_iDims-2; iDim >= 0; iDim--)
    {
        iIndex *= m_aiDim[iDim];
        iIndex += aiI[iDim];
    }
    return iIndex;
}
//----------------------------------------------------------------------------
Real InterpNDBSplineUniform::operator() (Real* afX)
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiBase[iDim] = (int)Math::Floor(afX[iDim]);

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        if ( m_aiOldBase[iDim] != m_aiBase[iDim] )
        {
            // switch to new local grid
            for (int k = 0; k < m_iDims; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid m_afData if necessary
            if ( m_oEvaluateCallback )
                EvaluateUnknownData();

            ComputeIntermediate();
            break;
        }
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
        SetPolynomial(0,afX[iDim]-m_aiBase[iDim],m_aafPoly[iDim]);

    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiOpI[iDim] = 0;

    Real fResult = 0.0f;
    int k;
    for (k = 0; k < m_iDp1ToN; k++)
    {
        Real fTemp = m_afInter[k];
        for (iDim = 0; iDim < m_iDims; iDim++)
            fTemp *= m_aafPoly[iDim][m_aiOpI[iDim]];
        fResult += fTemp;

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if ( ++m_aiOpI[iDim] <= m_iDegree )
                break;
            m_aiOpI[iDim] = 0;
        }
    }
    return fResult;
}
//----------------------------------------------------------------------------
Real InterpNDBSplineUniform::operator() (int* aiDx, Real* afX)
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiBase[iDim] = (int)Math::Floor(afX[iDim]);

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        if ( m_aiOldBase[iDim] != m_aiBase[iDim] )
        {
            // switch to new local grid
            for (int k = 0; k < m_iDims; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid m_afData if necessary
            if ( m_oEvaluateCallback )
                EvaluateUnknownData();

            ComputeIntermediate();
            break;
        }
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
        SetPolynomial(aiDx[iDim],afX[iDim]-m_aiBase[iDim],m_aafPoly[iDim]);

    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiOpJ[iDim] = aiDx[iDim];

    int iTemp = 1;
    for (iDim = 0; iDim < m_iDims-1; iDim++)
    {
        m_aiOpDelta[iDim] = iTemp*aiDx[iDim];
        iTemp *= m_iDp1;
    }

    Real fResult = 0.0f;
    int k = m_aiOpJ[m_iDims-1];
    for (iDim = m_iDims-2; iDim >= 0; iDim--)
    {
        k *= m_iDp1;
        k += m_aiOpJ[iDim];
    }
    for (/**/; k < m_iDp1ToN; k++)
    {
        Real fTemp = m_afInter[k];
        for (iDim = 0; iDim < m_iDims; iDim++)
            fTemp *= m_aafPoly[iDim][m_aiOpJ[iDim]];
        fResult += fTemp;

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if ( ++m_aiOpJ[iDim] <= m_iDegree )
                continue;
            m_aiOpJ[iDim] = aiDx[iDim];
            k += m_aiOpDelta[iDim];
        }
    }
    return fResult;
}
//----------------------------------------------------------------------------
void InterpNDBSplineUniform::EvaluateUnknownData ()
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiEvI[iDim] = m_aiGridMin[iDim];

    for (int j = 0; j < m_iDp1ToN; j++)
    {
        int iIndex = Index(m_aiEvI);
        if ( m_afData[iIndex] == Math::MAX_REAL )
            m_afData[iIndex] = m_oEvaluateCallback(iIndex);

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if ( ++m_aiEvI[iDim] <= m_aiGridMax[iDim] )
                break;
            m_aiEvI[iDim] = m_aiGridMin[iDim];
        }
    }
}
//----------------------------------------------------------------------------
void InterpNDBSplineUniform::ComputeIntermediate ()
{
    // fetch subblock of m_afData to cache
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
        m_aiCiLoop[iDim] = m_aiGridMin[iDim];
    int iIndex = Index(m_aiCiLoop);
    int k;
    for (k = 0; k < m_iDp1ToN; k++, iIndex++)
    {
        m_afCache[k] = m_afData[iIndex];

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if ( ++m_aiCiLoop[iDim] <= m_aiGridMax[iDim] )
                break;
            m_aiCiLoop[iDim] = m_aiGridMin[iDim];
            iIndex += m_aiCiDelta[iDim];
        }
    }

    // compute and save the intermediate product
    for (int i = 0, j = 0; i < m_iDp1ToN; i++)
    {
        Real fSum = 0.0f;
        for (k = 0; k < m_iDp1ToN; k += m_aiSkip[j], j += m_aiSkip[j])
            fSum += m_afProduct[j]*m_afCache[k];
        m_afInter[i] = fSum;
    }
}
//----------------------------------------------------------------------------
