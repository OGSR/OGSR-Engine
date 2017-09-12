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

#include "MgcInterp2DBSplineUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DBSplineUniform::Interp2DBSplineUniform (int iDegree, const int* aiDim,
    Real* afData)
    :
    InterpBSplineUniform(2,iDegree,aiDim,afData)
{
}
//----------------------------------------------------------------------------
Real Interp2DBSplineUniform::operator() (Real fX, Real fY)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    m_aiBase[1] = (int)Math::Floor(fY);
    for (int iDim = 0; iDim < 2; iDim++)
    {
        if ( m_aiOldBase[iDim] != m_aiBase[iDim] )
        {
            // switch to new local grid
            for (int k = 0; k < 2; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid data if necessary
            if ( m_oEvaluateCallback )
                EvaluateUnknownData();

            ComputeIntermediate();
            break;
        }
    }

    SetPolynomial(0,fX-m_aiBase[0],m_aafPoly[0]);
    SetPolynomial(0,fY-m_aiBase[1],m_aafPoly[1]);

    int aiI[2] = { 0, 0 };
    Real fResult = 0.0f;
    for (int k = aiI[0]+m_iDp1*aiI[1]; k < m_iDp1ToN; k++)
    {
        fResult += m_aafPoly[0][aiI[0]]*m_aafPoly[1][aiI[1]]*m_afInter[k];

        if ( ++aiI[0] <= m_iDegree )
            continue;
        aiI[0] = 0;

        aiI[1]++;
    }
    return fResult;
}
//----------------------------------------------------------------------------
Real Interp2DBSplineUniform::operator() (int iDx, int iDy, Real fX, Real fY)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    m_aiBase[1] = (int)Math::Floor(fY);
    for (int iDim = 0; iDim < 2; iDim++)
    {
        if ( m_aiOldBase[iDim] != m_aiBase[iDim] )
        {
            // switch to new local grid
            for (int k = 0; k < 2; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid data if necessary
            if ( m_oEvaluateCallback )
                EvaluateUnknownData();

            ComputeIntermediate();
            break;
        }
    }

    SetPolynomial(iDx,fX-m_aiBase[0],m_aafPoly[0]);
    SetPolynomial(iDy,fY-m_aiBase[1],m_aafPoly[1]);

    int aiI[2] = { iDx, iDy };
    int iIncr1 = iDx;
    Real fResult = 0.0f;
    for (int k = aiI[0]+m_iDp1*aiI[1]; k < m_iDp1ToN; k++)
    {
        fResult += m_aafPoly[0][aiI[0]]*m_aafPoly[1][aiI[1]]*m_afInter[k];

        if ( ++aiI[0] <= m_iDegree )
            continue;
        aiI[0] = iDx;
        k += iIncr1;

        aiI[1]++;
    }
    return fResult;
}
//----------------------------------------------------------------------------
void Interp2DBSplineUniform::EvaluateUnknownData ()
{
    for (int k1 = m_aiGridMin[1]; k1 <= m_aiGridMax[1]; k1++)
    {
        for (int k0 = m_aiGridMin[0]; k0 <= m_aiGridMax[0]; k0++)
        {
            int iIndex = Index(k0,k1);
            if ( m_afData[iIndex] == Math::MAX_REAL )
                m_afData[iIndex] = m_oEvaluateCallback(iIndex);
        }
    }
}
//----------------------------------------------------------------------------
void Interp2DBSplineUniform::ComputeIntermediate ()
{
    // fetch subblock of data to cache
    int iDelta0 = m_aiDim[0] - m_iDp1;
    int aiLoop[2];
    for (int iDim = 0; iDim < 2; iDim++)
        aiLoop[iDim] = m_aiGridMin[iDim];
    int iIndex = Index(aiLoop[0],aiLoop[1]);
    int k;
    for (k = 0; k < m_iDp1ToN; k++, iIndex++)
    {
        m_afCache[k] = m_afData[iIndex];

        if ( ++aiLoop[0] <= m_aiGridMax[0] )
            continue;
        aiLoop[0] = m_aiGridMin[0];
        iIndex += iDelta0;

        aiLoop[1]++;
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
