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

#include "MgcInterp3DBSplineUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp3DBSplineUniform::Interp3DBSplineUniform (int iDegree, const int* aiDim,
    Real* afData)
    :
    InterpBSplineUniform(3,iDegree,aiDim,afData)
{
}
//----------------------------------------------------------------------------
Real Interp3DBSplineUniform::operator() (Real fX, Real fY, Real fZ)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    m_aiBase[1] = (int)Math::Floor(fY);
    m_aiBase[2] = (int)Math::Floor(fZ);
    for (int iDim = 0; iDim < 3; iDim++)
    {
        if ( m_aiOldBase[iDim] != m_aiBase[iDim] )
        {
            // switch to new local grid
            for (int k = 0; k < 3; k++)
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
    SetPolynomial(0,fZ-m_aiBase[2],m_aafPoly[2]);

    int aiI[3] = { 0, 0, 0 };
    Real fResult = 0.0f;
    for (int k = 0; k < m_iDp1ToN; k++)
    {
        fResult += m_aafPoly[0][aiI[0]]*m_aafPoly[1][aiI[1]]*
            m_aafPoly[2][aiI[2]]*m_afInter[k];

        if ( ++aiI[0] <= m_iDegree )
            continue;
        aiI[0] = 0;

        if ( ++aiI[1] <= m_iDegree )
            continue;
        aiI[1] = 0;

        aiI[2]++;
    }
    return fResult;
}
//----------------------------------------------------------------------------
Real Interp3DBSplineUniform::operator() (int iDx, int iDy, int iDz, Real fX,
    Real fY, Real fZ)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    m_aiBase[1] = (int)Math::Floor(fY);
    m_aiBase[2] = (int)Math::Floor(fZ);
    for (int d = 0; d < 3; d++)
    {
        if ( m_aiOldBase[d] != m_aiBase[d] )
        {
            // switch to new local grid
            for (int k = 0; k < 3; k++)
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
    SetPolynomial(iDz,fZ-m_aiBase[2],m_aafPoly[2]);

    int aiI[3] = { iDx, iDy, iDz };
    int aiDelta[2] = { iDx, m_iDp1*iDy };
    Real fResult = 0.0f;
    for (int k = aiI[0]+m_iDp1*(aiI[1]+m_iDp1*aiI[2]); k < m_iDp1ToN; k++)
    {
        fResult +=  m_aafPoly[0][aiI[0]]*m_aafPoly[1][aiI[1]]*
            m_aafPoly[2][aiI[2]]*m_afInter[k];

        if ( ++aiI[0] <= m_iDegree )
            continue;
        aiI[0] = iDx;
        k += aiDelta[0];

        if ( ++aiI[1] <= m_iDegree )
            continue;
        aiI[1] = iDy;
        k += aiDelta[1];

        aiI[2]++;
    }
    return fResult;
}
//----------------------------------------------------------------------------
void Interp3DBSplineUniform::EvaluateUnknownData ()
{
    for (int k2 = m_aiGridMin[2]; k2 <= m_aiGridMax[2]; k2++)
    {
        for (int k1 = m_aiGridMin[1]; k1 <= m_aiGridMax[1]; k1++)
        {
            for (int k0 = m_aiGridMin[0]; k0 <= m_aiGridMax[0]; k0++)
            {
                int iIndex = Index(k0,k1,k2);
                if ( m_afData[iIndex] == Math::MAX_REAL )
                    m_afData[iIndex] = m_oEvaluateCallback(iIndex);
            }
        }
    }
}
//----------------------------------------------------------------------------
void Interp3DBSplineUniform::ComputeIntermediate ()
{
    // fetch subblock of data to cache
    int aiDelta[2] = { m_aiDim[0]-m_iDp1, m_aiDim[0]*(m_aiDim[1]-m_iDp1) };
    int aiLoop[3];
    for (int iDim = 0; iDim < 3; iDim++)
        aiLoop[iDim] = m_aiGridMin[iDim];
    int iIndex = Index(aiLoop[0],aiLoop[1],aiLoop[2]);
    int k;
    for (k = 0; k < m_iDp1ToN; k++, iIndex++)
    {
        m_afCache[k] = m_afData[iIndex];

        if ( ++aiLoop[0] <= m_aiGridMax[0] )
            continue;
        aiLoop[0] = m_aiGridMin[0];
        iIndex += aiDelta[0];

        if ( ++aiLoop[1] <= m_aiGridMax[1] )
            continue;
        aiLoop[1] = m_aiGridMin[1];
        iIndex += aiDelta[1];

        aiLoop[2]++;
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
