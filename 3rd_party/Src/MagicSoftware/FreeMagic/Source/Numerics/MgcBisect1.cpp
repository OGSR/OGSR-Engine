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

#include "MgcBisect1.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Bisect1::Bisect1 (Function oF, int iMaxLevel, Real fTolerance)
{
    m_oF = oF;
    m_iMaxLevel = iMaxLevel;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
bool Bisect1::Bisect (Real fX0, Real fX1, Real& rfRoot)
{
    // test two endpoints
    Real fF0 = m_oF(fX0);
    if ( Math::FAbs(fF0) <= m_fTolerance )
    {
        rfRoot = fX0;
        return true;
    }

    Real fF1 = m_oF(fX1);
    if ( Math::FAbs(fF1) <= m_fTolerance )
    {
        rfRoot = fX1;
        return true;
    }

    if ( fF0*fF1 > 0.0f )
        return false;

    for (int iLevel = 0; iLevel < m_iMaxLevel; iLevel++)
    {
        Real fXm = 0.5f*(fX0+fX1);
        Real fFm = m_oF(fXm);
        if ( Math::FAbs(fFm) <= m_fTolerance )
        {
            rfRoot = fXm;
            return true;
        }

        if ( fF0*fFm < 0.0f )
        {
            fX1 = fXm;
            fF1 = fFm;
        }
        else if ( fF1*fFm < 0.0f )
        {
            fX0 = fXm;
            fF0 = fFm;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

