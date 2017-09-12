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

#include "MgcInterp1DBSplineUniform.h"
#include "MgcMath.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp1DBSplineUniform::Interp1DBSplineUniform (int iDegree, int iDim,
    Real* afData)
    :
    InterpBSplineUniform(1,iDegree,&iDim,afData)
{
}
//----------------------------------------------------------------------------
Real Interp1DBSplineUniform::operator() (Real fX)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    if ( m_aiOldBase[0] != m_aiBase[0] )
    {
        // switch to new local grid
        m_aiOldBase[0] = m_aiBase[0];
        m_aiGridMin[0] = m_aiBase[0] - 1;
        m_aiGridMax[0] = m_aiGridMin[0] + m_iDegree;

        // fill in missing grid data if necessary
        if ( m_oEvaluateCallback )
            EvaluateUnknownData();

        ComputeIntermediate();
    }

    SetPolynomial(0,fX-m_aiBase[0],m_aafPoly[0]);

    Real fResult = 0.0f;
    for (int k = 0; k <= m_iDegree; k++)
        fResult += m_aafPoly[0][k]*m_afInter[k];
    return fResult;
}
//----------------------------------------------------------------------------
Real Interp1DBSplineUniform::operator() (int iDx, Real fX)
{
    m_aiBase[0] = (int)Math::Floor(fX);
    if ( m_aiOldBase[0] != m_aiBase[0] )
    {
        // switch to new local grid
        m_aiOldBase[0] = m_aiBase[0];
        m_aiGridMin[0] = m_aiBase[0] - 1;
        m_aiGridMax[0] = m_aiGridMin[0] + m_iDegree;

        // fill in missing grid data if necessary
        if ( m_oEvaluateCallback )
            EvaluateUnknownData();

        ComputeIntermediate();
    }

    SetPolynomial(iDx,fX-m_aiBase[0],m_aafPoly[0]);

    Real fResult = 0.0f;
    for (int k = iDx; k <= m_iDegree; k++)
        fResult += m_aafPoly[0][k]*m_afInter[k];
    return fResult;
}
//----------------------------------------------------------------------------
void Interp1DBSplineUniform::EvaluateUnknownData ()
{
    for (int k = m_aiGridMin[0]; k <= m_aiGridMax[0]; k++)
    {
        if ( m_afData[k] == Math::MAX_REAL )
            m_afData[k] = m_oEvaluateCallback(k);
    }
}
//----------------------------------------------------------------------------
void Interp1DBSplineUniform::ComputeIntermediate ()
{
    for (int k = 0; k <= m_iDegree; k++)
    {
        m_afInter[k] = 0.0f;
        for (int i = 0, j = m_aiBase[0]-1; i <= m_iDegree; i++, j++)
            m_afInter[k] += m_afData[j]*m_aafMatrix[i][k];
    }
}
//----------------------------------------------------------------------------
