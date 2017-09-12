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

#include "MgcBisect3.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Bisect3::Bisect3 (Function oF, Function oG, Function oH, int iMaxLevel,
    Real fTolerance)
{
    m_oF = oF;
    m_oG = oG;
    m_oH = oH;
    m_iMaxLevel = iMaxLevel;
    m_iLevel = 0;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
#define ZeroTest(fX,fY,fZ,fF,fG,fH,fXRoot,fYRoot,fZRoot) \
    fF = m_oF(fX,fY,fZ); \
    fG = m_oG(fX,fY,fZ); \
    fH = m_oH(fX,fY,fZ); \
    if ( Math::FAbs(fF) <= m_fTolerance \
    &&   Math::FAbs(fG) <= m_fTolerance \
    &&   Math::FAbs(fH) <= m_fTolerance ) \
    { \
        fXRoot = fX; \
        fYRoot = fY; \
        fZRoot = fZ; \
        m_iLevel--; \
        return true; \
    }
//----------------------------------------------------------------------------
#define AddNode(fX,fY,fZ,fF,fG,fH) \
    m_pkTemp = new BisectNode; \
    m_pkTemp->m_fX = fX; \
    m_pkTemp->m_fY = fY; \
    m_pkTemp->m_fZ = fZ; \
    m_pkTemp->m_fF = fF; \
    m_pkTemp->m_fG = fG; \
    m_pkTemp->m_fH = fH;
//----------------------------------------------------------------------------
bool Bisect3::Bisect (Real fX0, Real fY0, Real fZ0, Real fX1,
    Real fY1, Real fZ1, Real& rfXRoot, Real& rfYRoot, Real& rfZRoot)
{
    // test eight corner values
    ZeroTest(fX0,fY0,fZ0,m_fF000,m_fG000,m_fH000,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX1,fY0,fZ0,m_fF100,m_fG100,m_fH100,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX0,fY1,fZ0,m_fF010,m_fG010,m_fH010,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX1,fY1,fZ0,m_fF110,m_fG110,m_fH110,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX0,fY0,fZ1,m_fF001,m_fG001,m_fH001,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX1,fY0,fZ1,m_fF101,m_fG101,m_fH101,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX0,fY1,fZ1,m_fF011,m_fG011,m_fH011,rfXRoot,rfYRoot,rfZRoot);
    ZeroTest(fX1,fY1,fZ1,m_fF111,m_fG111,m_fH111,rfXRoot,rfYRoot,rfZRoot);

    // build initial oct

    // add pkN000
    m_pkGraph = new BisectNode;
    m_pkGraph->m_fX = fX0;
    m_pkGraph->m_fY = fY0;
    m_pkGraph->m_fZ = fZ0;
    m_pkGraph->m_fF = m_fF000;
    m_pkGraph->m_fG = m_fG000;
    m_pkGraph->m_fH = m_fH000;

    // add pkN100
    AddNode(fX1,fY0,fZ0,m_fF100,m_fG100,m_fH100);
    m_pkTemp->m_pkXNext = NULL;
    m_pkGraph->m_pkXNext = m_pkTemp;

    // add pkN010
    AddNode(fX0,fY1,fZ0,m_fF010,m_fG010,m_fH010);
    m_pkTemp->m_pkYNext = NULL;
    m_pkGraph->m_pkYNext = m_pkTemp;

    // add pkN110
    AddNode(fX1,fY1,fZ0,m_fF110,m_fG110,m_fH110);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = NULL;
    m_pkGraph->m_pkXNext->m_pkYNext = m_pkTemp;
    m_pkGraph->m_pkYNext->m_pkXNext = m_pkTemp;

    // add pkN001
    AddNode(fX0,fY1,fZ1,m_fF001,m_fG001,m_fH001);
    m_pkTemp->m_pkZNext = NULL;
    m_pkGraph->m_pkZNext = m_pkTemp;

    // add pkN101
    AddNode(fX1,fY0,fZ1,m_fF101,m_fG101,m_fH101);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkZNext = NULL;
    m_pkGraph->m_pkXNext->m_pkZNext = m_pkTemp;
    m_pkGraph->m_pkZNext->m_pkXNext = m_pkTemp;

    // add pkN011
    AddNode(fX0,fY1,fZ1,m_fF011,m_fG011,m_fH011);
    m_pkTemp->m_pkYNext = NULL;
    m_pkTemp->m_pkZNext = NULL;
    m_pkGraph->m_pkYNext->m_pkZNext = m_pkTemp;
    m_pkGraph->m_pkZNext->m_pkYNext = m_pkTemp;

    // add pkN111
    AddNode(fX1,fY1,fZ1,m_fF111,m_fG111,m_fH111);
    m_pkGraph->m_pkXNext->m_pkYNext->m_pkZNext = m_pkTemp;
    m_pkGraph->m_pkYNext->m_pkXNext->m_pkZNext = m_pkTemp;
    m_pkGraph->m_pkXNext->m_pkZNext->m_pkYNext = m_pkTemp;

    bool bResult = BisectRecurse(m_pkGraph);
    if ( bResult )
    {
        rfXRoot = m_fXRoot;
        rfYRoot = m_fYRoot;
        rfZRoot = m_fZRoot;
    }

    // remove remaining oct from m_pkGraph
    delete m_pkGraph->m_pkXNext->m_pkYNext->m_pkZNext;
    delete m_pkGraph->m_pkXNext->m_pkZNext;
    delete m_pkGraph->m_pkYNext->m_pkZNext;
    delete m_pkGraph->m_pkZNext;
    delete m_pkGraph->m_pkXNext->m_pkYNext;
    delete m_pkGraph->m_pkXNext;
    delete m_pkGraph->m_pkYNext;
    delete m_pkGraph;

    return bResult;
}
//----------------------------------------------------------------------------
bool Bisect3::BisectRecurse (BisectNode* pkN000)
{
    if ( ++m_iLevel == m_iMaxLevel )
    {
        m_iLevel--;
        return false;
    }

    BisectNode* pkN100 = pkN000->m_pkXNext;
    BisectNode* pkN010 = pkN000->m_pkYNext;
    BisectNode* pkN110 = pkN100->m_pkYNext;
    BisectNode* pkN001 = pkN000->m_pkZNext;
    BisectNode* pkN101 = pkN001->m_pkXNext;
    BisectNode* pkN011 = pkN001->m_pkYNext;
    BisectNode* pkN111 = pkN101->m_pkYNext;

    m_iNetSign = int(Math::Sign(pkN000->m_fF) + Math::Sign(pkN010->m_fF)
        + Math::Sign(pkN100->m_fF) + Math::Sign(pkN110->m_fF)
        + Math::Sign(pkN001->m_fF) + Math::Sign(pkN011->m_fF)
        + Math::Sign(pkN101->m_fF) + Math::Sign(pkN111->m_fF));

    if ( abs(m_iNetSign) == 8 )
    {
        // F has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = int(Math::Sign(pkN000->m_fG) + Math::Sign(pkN010->m_fG)
        + Math::Sign(pkN100->m_fG) + Math::Sign(pkN110->m_fG)
        + Math::Sign(pkN001->m_fG) + Math::Sign(pkN011->m_fG)
        + Math::Sign(pkN101->m_fG) + Math::Sign(pkN111->m_fG));

    if ( abs(m_iNetSign) == 8 )
    {
        // G has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = int(Math::Sign(pkN000->m_fH) + Math::Sign(pkN010->m_fH)
        + Math::Sign(pkN100->m_fH) + Math::Sign(pkN110->m_fH)
        + Math::Sign(pkN001->m_fH) + Math::Sign(pkN011->m_fH)
        + Math::Sign(pkN101->m_fH) + Math::Sign(pkN111->m_fH));

    if ( abs(m_iNetSign) == 8 )
    {
        // H has same sign at corners
        m_iLevel--;
        return false;
    }

    // bisect the oct
    m_fX0 = pkN000->m_fX;
    m_fY0 = pkN000->m_fY;
    m_fZ0 = pkN000->m_fZ;
    m_fX1 = pkN111->m_fX;
    m_fY1 = pkN111->m_fY;
    m_fZ1 = pkN111->m_fZ;
    m_fXm = 0.5f*(m_fX0+m_fX1);
    m_fYm = 0.5f*(m_fY0+m_fY1);
    m_fZm = 0.5f*(m_fZ0+m_fZ1);

    ZeroTest(m_fXm,m_fY1,m_fZ1,m_fFm11,m_fGm11,m_fHm11,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 011,111
    ZeroTest(m_fX1,m_fYm,m_fZ1,m_fF1m1,m_fG1m1,m_fH1m1,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 101,111
    ZeroTest(m_fX1,m_fY1,m_fZm,m_fF11m,m_fG11m,m_fH11m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 110,111

    ZeroTest(m_fXm,m_fY1,m_fZ0,m_fFm10,m_fGm10,m_fHm10,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 010,110
    ZeroTest(m_fX1,m_fYm,m_fZ0,m_fF1m0,m_fG1m0,m_fH1m0,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 100,110
    ZeroTest(m_fX1,m_fY0,m_fZm,m_fF10m,m_fG10m,m_fH10m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 100,101
    ZeroTest(m_fXm,m_fY0,m_fZ1,m_fFm01,m_fGm01,m_fHm01,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 001,101
    ZeroTest(m_fX0,m_fYm,m_fZ1,m_fF0m1,m_fG0m1,m_fH0m1,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 001,011
    ZeroTest(m_fX0,m_fY1,m_fZm,m_fF01m,m_fG01m,m_fH01m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 010,011

    ZeroTest(m_fXm,m_fY0,m_fZ0,m_fFm00,m_fGm00,m_fHm00,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 000,100
    ZeroTest(m_fX0,m_fYm,m_fZ0,m_fF0m0,m_fG0m0,m_fH0m0,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 000,010
    ZeroTest(m_fX0,m_fY0,m_fZm,m_fF00m,m_fG00m,m_fH00m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // edge 000,001

    ZeroTest(m_fX1,m_fYm,m_fZm,m_fF1mm,m_fG1mm,m_fH1mm,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 110,100,101,111
    ZeroTest(m_fXm,m_fY1,m_fZm,m_fFm1m,m_fGm1m,m_fHm1m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 010,110,111,011
    ZeroTest(m_fXm,m_fYm,m_fZ1,m_fFmm1,m_fGmm1,m_fHmm1,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 001,101,111,011
    ZeroTest(m_fX0,m_fYm,m_fZm,m_fF0mm,m_fG0mm,m_fH0mm,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 000,010,011,001
    ZeroTest(m_fXm,m_fY0,m_fZm,m_fFm0m,m_fGm0m,m_fHm0m,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 000,100,001,101
    ZeroTest(m_fXm,m_fYm,m_fZ0,m_fFmm0,m_fGmm0,m_fHmm0,
        m_fXRoot,m_fYRoot,m_fZRoot);  // face 000,100,110,010

    ZeroTest(m_fXm,m_fYm,m_fZm,m_fFmmm,m_fGmmm,m_fHmmm,
        m_fXRoot,m_fYRoot,m_fZRoot);  // center

    // edge 011,111
    AddNode(m_fXm,m_fY1,m_fZ1,m_fFm11,m_fGm11,m_fHm11);
    m_pkTemp->m_pkXNext = pkN111;
    m_pkTemp->m_pkYNext = NULL;
    m_pkTemp->m_pkZNext = NULL;
    pkN011->m_pkXNext = m_pkTemp;

    // edge 101,111
    AddNode(m_fX1,m_fYm,m_fZ1,m_fF1m1,m_fG1m1,m_fH1m1);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = pkN111;
    m_pkTemp->m_pkZNext = NULL;
    pkN101->m_pkYNext = m_pkTemp;

    // edge 110,111
    AddNode(m_fX1,m_fY1,m_fZm,m_fF11m,m_fG11m,m_fH11m);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = NULL;
    m_pkTemp->m_pkZNext = pkN111;
    pkN110->m_pkZNext = m_pkTemp;

    // edge 010,110
    AddNode(m_fXm,m_fY1,m_fZ0,m_fFm10,m_fGm10,m_fHm10);
    m_pkTemp->m_pkXNext = pkN110;
    m_pkTemp->m_pkYNext = NULL;
    pkN010->m_pkXNext = m_pkTemp;

    // edge 100,110
    AddNode(m_fX1,m_fYm,m_fZ1,m_fF1m0,m_fG1m0,m_fH1m0);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = pkN110;
    pkN100->m_pkYNext = m_pkTemp;

    // edge 100,101
    AddNode(m_fX1,m_fY0,m_fZm,m_fF10m,m_fG10m,m_fH10m);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkZNext = pkN101;
    pkN100->m_pkZNext = m_pkTemp;

    // edge 001,101
    AddNode(m_fXm,m_fY0,m_fZ1,m_fFm01,m_fGm01,m_fHm01);
    m_pkTemp->m_pkXNext = pkN101;
    m_pkTemp->m_pkZNext = NULL;
    pkN001->m_pkXNext = m_pkTemp;

    // edge 001,011
    AddNode(m_fX0,m_fYm,m_fZ1,m_fF0m1,m_fG0m1,m_fH0m1);
    m_pkTemp->m_pkYNext = pkN011;
    m_pkTemp->m_pkZNext = NULL;
    pkN001->m_pkYNext = m_pkTemp;

    // edge 010,011
    AddNode(m_fX0,m_fY1,m_fZm,m_fF01m,m_fG01m,m_fH01m);
    m_pkTemp->m_pkYNext = NULL;
    m_pkTemp->m_pkZNext = pkN011;
    pkN010->m_pkZNext = m_pkTemp;

    // edge 000,100
    AddNode(m_fXm,m_fY0,m_fZ0,m_fFm00,m_fGm00,m_fHm00);
    m_pkTemp->m_pkXNext = pkN100;
    pkN000->m_pkXNext = m_pkTemp;

    // edge 000,010
    AddNode(m_fX0,m_fYm,m_fZ0,m_fF0m0,m_fG0m0,m_fH0m0);
    m_pkTemp->m_pkYNext = pkN010;
    pkN000->m_pkYNext = m_pkTemp;

    // edge 000,001
    AddNode(m_fX0,m_fY0,m_fZm,m_fF00m,m_fG00m,m_fH00m);
    m_pkTemp->m_pkZNext = pkN001;
    pkN000->m_pkZNext = m_pkTemp;

    // face 110,100,101,111
    AddNode(m_fX1,m_fYm,m_fZm,m_fF11m,m_fG11m,m_fH11m);
    m_pkTemp->m_pkXNext = NULL;
    m_pkTemp->m_pkYNext = pkN110->m_pkZNext;
    m_pkTemp->m_pkZNext = pkN101->m_pkYNext;
    pkN100->m_pkYNext->m_pkZNext = m_pkTemp;
    pkN100->m_pkZNext->m_pkYNext = m_pkTemp;

    // face 010,110,111,011
    AddNode(m_fXm,m_fY1,m_fZm,m_fFm1m,m_fGm1m,m_fHm1m);
    m_pkTemp->m_pkXNext = pkN110->m_pkZNext;
    m_pkTemp->m_pkYNext = NULL;
    m_pkTemp->m_pkZNext = pkN011->m_pkXNext;
    pkN010->m_pkXNext->m_pkZNext = m_pkTemp;
    pkN010->m_pkZNext->m_pkXNext = m_pkTemp;

    // face 001,101,111,011
    AddNode(m_fXm,m_fYm,m_fZ1,m_fFmm1,m_fGmm1,m_fHmm1);
    m_pkTemp->m_pkXNext = pkN101->m_pkYNext;
    m_pkTemp->m_pkYNext = pkN011->m_pkXNext;
    m_pkTemp->m_pkZNext = NULL;
    pkN001->m_pkXNext->m_pkYNext = m_pkTemp;
    pkN001->m_pkYNext->m_pkXNext = m_pkTemp;

    // face 000,010,011,001
    AddNode(m_fX0,m_fYm,m_fZm,m_fF0mm,m_fG0mm,m_fH0mm);
    m_pkTemp->m_pkYNext = pkN010->m_pkZNext;
    m_pkTemp->m_pkZNext = pkN001->m_pkYNext;
    pkN000->m_pkYNext->m_pkZNext = m_pkTemp;
    pkN000->m_pkZNext->m_pkYNext = m_pkTemp;

    // face 000,100,001,101
    AddNode(m_fXm,m_fY0,m_fZm,m_fFm0m,m_fGm0m,m_fHm0m);
    m_pkTemp->m_pkXNext = pkN100->m_pkZNext;
    m_pkTemp->m_pkZNext = pkN001->m_pkXNext;
    pkN000->m_pkXNext->m_pkZNext = m_pkTemp;
    pkN000->m_pkZNext->m_pkXNext = m_pkTemp;

    // face 000,100,110,010
    AddNode(m_fXm,m_fYm,m_fZ0,m_fFmm0,m_fGmm0,m_fHmm0);
    m_pkTemp->m_pkXNext = pkN100->m_pkYNext;
    m_pkTemp->m_pkYNext = pkN010->m_pkXNext;
    pkN000->m_pkXNext->m_pkYNext = m_pkTemp;
    pkN000->m_pkYNext->m_pkXNext = m_pkTemp;

    // center
    AddNode(m_fXm,m_fYm,m_fZm,m_fFmmm,m_fGmmm,m_fHmmm);
    m_pkTemp->m_pkXNext = pkN100->m_pkYNext->m_pkZNext;
    m_pkTemp->m_pkYNext = pkN010->m_pkXNext->m_pkZNext;
    m_pkTemp->m_pkZNext = pkN001->m_pkXNext->m_pkYNext;
    pkN000->m_pkXNext->m_pkYNext->m_pkZNext = m_pkTemp;
    pkN000->m_pkXNext->m_pkZNext->m_pkYNext = m_pkTemp;
    pkN000->m_pkYNext->m_pkZNext->m_pkXNext = m_pkTemp;

    // Search the subocts for roots
    bool bResult =
        BisectRecurse(pkN000) ||
        BisectRecurse(pkN000->m_pkXNext) ||
        BisectRecurse(pkN000->m_pkYNext) ||
        BisectRecurse(pkN000->m_pkXNext->m_pkYNext) ||
        BisectRecurse(pkN000->m_pkZNext) ||
        BisectRecurse(pkN000->m_pkZNext->m_pkXNext) ||
        BisectRecurse(pkN000->m_pkZNext->m_pkYNext) ||
        BisectRecurse(pkN000->m_pkZNext->m_pkXNext->m_pkYNext);

    // entire suboct check failed, remove the nodes that were added

    // center
    delete pkN000->m_pkXNext->m_pkYNext->m_pkZNext;

    // faces
    delete pkN000->m_pkXNext->m_pkYNext;
    delete pkN000->m_pkYNext->m_pkZNext;
    delete pkN000->m_pkXNext->m_pkZNext;
    delete pkN001->m_pkXNext->m_pkYNext;
    delete pkN010->m_pkXNext->m_pkZNext;
    delete pkN100->m_pkYNext->m_pkZNext;

    // edges
    delete pkN000->m_pkXNext;  pkN000->m_pkXNext = pkN100;
    delete pkN000->m_pkYNext;  pkN000->m_pkYNext = pkN010;
    delete pkN000->m_pkZNext;  pkN000->m_pkZNext = pkN001;
    delete pkN001->m_pkYNext;  pkN001->m_pkYNext = pkN011;
    delete pkN001->m_pkXNext;  pkN001->m_pkXNext = pkN101;
    delete pkN010->m_pkZNext;  pkN010->m_pkZNext = pkN011;
    delete pkN100->m_pkZNext;  pkN100->m_pkZNext = pkN101;
    delete pkN010->m_pkXNext;  pkN010->m_pkXNext = pkN110;
    delete pkN100->m_pkYNext;  pkN100->m_pkYNext = pkN110;
    delete pkN011->m_pkXNext;  pkN011->m_pkXNext = pkN111;
    delete pkN101->m_pkYNext;  pkN101->m_pkYNext = pkN111;
    delete pkN110->m_pkZNext;  pkN110->m_pkZNext = pkN111;

    m_iLevel--;
    return bResult;
}
//----------------------------------------------------------------------------

