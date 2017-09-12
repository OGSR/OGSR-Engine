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

#include "MgcBisect2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Bisect2::Bisect2 (Function oF, Function oG, int iMaxLevel, Real fTolerance)
{
    m_oF = oF;
    m_oG = oG;
    m_iMaxLevel = iMaxLevel;
    m_iLevel = 0;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
#define ZeroTest(fX,fY,fF,fG,fXRoot,fYRoot) \
    fF = m_oF(fX,fY); \
    fG = m_oG(fX,fY); \
    if ( Math::FAbs(fF) <= m_fTolerance && Math::FAbs(fG) <= m_fTolerance ) \
    { \
        fXRoot = fX; \
        fYRoot = fY; \
        m_iLevel--; \
        return true; \
    }
//----------------------------------------------------------------------------
#define AddNode(fX,fY,fF,fG) \
    m_pkTemp = new BisectNode; \
    m_pkTemp->m_fX = fX; \
    m_pkTemp->m_fY = fY; \
    m_pkTemp->m_fF = fF; \
    m_pkTemp->m_fG = fG;
//----------------------------------------------------------------------------
bool Bisect2::Bisect (Real fX0, Real fY0, Real fX1, Real fY1,
    Real& rfXRoot, Real& rfYRoot)
{
    // test four corner values
    ZeroTest(fX0,fY0,m_fF00,m_fG00,rfXRoot,rfYRoot);
    ZeroTest(fX1,fY0,m_fF10,m_fG10,rfXRoot,rfYRoot);
    ZeroTest(fX0,fY1,m_fF01,m_fG01,rfXRoot,rfYRoot);
    ZeroTest(fX1,fY1,m_fF11,m_fG11,rfXRoot,rfYRoot);

    // build initial quad

    // add pkN00
    m_pkGraph = new BisectNode;
    m_pkGraph->m_fX = fX0;
    m_pkGraph->m_fY = fY0;
    m_pkGraph->m_fF = m_fF00;
    m_pkGraph->m_fG = m_fG00;

    // add pkN10
    AddNode(fX1,fY0,m_fF10,m_fG10);
    m_pkTemp->m_pkXNext = NULL;
    m_pkGraph->m_pkXNext = m_pkTemp;

    // add pkN01
    AddNode(fX0,fY1,m_fF01,m_fG01);
    m_pkTemp->m_pkYNext = NULL;
    m_pkGraph->m_pkYNext = m_pkTemp;

    // add pkN11
    AddNode(fX1,fY1,m_fF11,m_fG11);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = NULL;
    m_pkGraph->m_pkXNext->m_pkYNext = m_pkTemp;
    m_pkGraph->m_pkYNext->m_pkXNext = m_pkTemp;

    m_iLevel = 0;
    bool bResult = BisectRecurse(m_pkGraph);
    if ( bResult )
    {
        rfXRoot = m_fXRoot;
        rfYRoot = m_fYRoot;
    }

    // remove remaining quad from m_pkGraph
    delete m_pkGraph->m_pkXNext->m_pkYNext;
    delete m_pkGraph->m_pkXNext;
    delete m_pkGraph->m_pkYNext;
    delete m_pkGraph;

    return bResult;
}
//----------------------------------------------------------------------------
bool Bisect2::BisectRecurse (BisectNode* pkN00)
{
    if ( ++m_iLevel == m_iMaxLevel )
    {
        m_iLevel--;
        return false;
    }

    BisectNode* pkN10 = pkN00->m_pkXNext;
    BisectNode* pkN11 = pkN10->m_pkYNext;
    BisectNode* pkN01 = pkN00->m_pkYNext;

    m_iNetSign = int(Math::Sign(pkN00->m_fF) + Math::Sign(pkN01->m_fF) +
        Math::Sign(pkN10->m_fF) + Math::Sign(pkN11->m_fF));

    if ( abs(m_iNetSign) == 4 )
    {
        // F has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = int(Math::Sign(pkN00->m_fG) + Math::Sign(pkN01->m_fG) +
        Math::Sign(pkN10->m_fG) + Math::Sign(pkN11->m_fG));

    if ( abs(m_iNetSign) == 4 )
    {
        // G has same sign at corners
        m_iLevel--;
        return false;
    }

    // bisect the quad
    m_fX0 = pkN00->m_fX;
    m_fY0 = pkN00->m_fY;
    m_fX1 = pkN11->m_fX;
    m_fY1 = pkN11->m_fY;
    m_fXm = 0.5f*(m_fX0+m_fX1);
    m_fYm = 0.5f*(m_fY0+m_fY1);

    ZeroTest(m_fX1,m_fYm,m_fF1m,m_fG1m,m_fXRoot,m_fYRoot);  // r edge 10,11
    ZeroTest(m_fXm,m_fY1,m_fFm1,m_fGm1,m_fXRoot,m_fYRoot);  // b edge 01,11
    ZeroTest(m_fXm,m_fY0,m_fFm0,m_fGm0,m_fXRoot,m_fYRoot);  // t edge 00,10
    ZeroTest(m_fX0,m_fYm,m_fF0m,m_fG0m,m_fXRoot,m_fYRoot);  // l edge 00,01
    ZeroTest(m_fXm,m_fYm,m_fFmm,m_fGmm,m_fXRoot,m_fYRoot);  // center

    // right bisector
    AddNode(m_fX1,m_fYm,m_fF1m,m_fG1m);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = pkN11;
    pkN10->m_pkYNext = m_pkTemp;

    // bottom bisector
    AddNode(m_fXm,m_fY1,m_fFm1,m_fGm1);
    m_pkTemp->m_pkXNext = pkN11;
    m_pkTemp->m_pkYNext = NULL;
    pkN01->m_pkXNext = m_pkTemp;

    // top bisector
    AddNode(m_fXm,m_fY0,m_fFm0,m_fGm0);
    m_pkTemp->m_pkXNext = pkN10;
    pkN00->m_pkXNext = m_pkTemp;

    // left bisector
    AddNode(m_fX0,m_fYm,m_fF0m,m_fG0m);
    m_pkTemp->m_pkYNext = pkN01;
    pkN00->m_pkYNext = m_pkTemp;

    // middle bisector
    AddNode(m_fXm,m_fYm,m_fFmm,m_fGmm);
    m_pkTemp->m_pkXNext = pkN10->m_pkYNext;
    m_pkTemp->m_pkYNext = pkN01->m_pkXNext;
    pkN00->m_pkXNext->m_pkYNext = m_pkTemp;
    pkN00->m_pkYNext->m_pkXNext = m_pkTemp;

    // Search the subquads for roots
    bool bResult =
        BisectRecurse(pkN00) ||
        BisectRecurse(pkN00->m_pkXNext) ||
        BisectRecurse(pkN00->m_pkYNext) ||
        BisectRecurse(pkN00->m_pkXNext->m_pkYNext);

    // entire subquad check failed, remove the nodes that were added

    // center
    delete pkN00->m_pkXNext->m_pkYNext;

    // edges
    delete pkN00->m_pkXNext;
    pkN00->m_pkXNext = pkN10;
    delete pkN00->m_pkYNext;
    pkN00->m_pkYNext = pkN01;
    delete pkN01->m_pkXNext;
    pkN01->m_pkXNext = pkN11;
    delete pkN10->m_pkYNext;
    pkN10->m_pkYNext = pkN11;

    m_iLevel--;
    return bResult;
}
//----------------------------------------------------------------------------

