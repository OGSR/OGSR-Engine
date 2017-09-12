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

#include "MgcIntegrate.h"
#include "MgcSingleCurve2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
SingleCurve2::SingleCurve2 (Real fTMin, Real fTMax)
    :
    Curve2(fTMin,fTMax)
{
}
//----------------------------------------------------------------------------
Real SingleCurve2::GetSpeedWithData (Real fTime, void* pvData)
{
    return ((Curve2*)pvData)->GetSpeed(fTime);
}
//----------------------------------------------------------------------------
Real SingleCurve2::GetLength (Real fT0, Real fT1) const
{
    assert( m_fTMin <= fT0 && fT0 <= m_fTMax );
    assert( m_fTMin <= fT1 && fT1 <= m_fTMax );
    assert( fT0 <= fT1 );

    return Integrate::RombergIntegral(fT0,fT1,GetSpeedWithData,(void*)this);
}
//----------------------------------------------------------------------------
Real SingleCurve2::GetTime (Real fLength, int iIterations,
    Real fTolerance) const
{
    if ( fLength <= 0.0f )
        return m_fTMin;

    if ( fLength >= GetTotalLength() )
        return m_fTMax;

    // initial guess for Newton's method
    Real fRatio = fLength/GetTotalLength();
    Real fOmRatio = 1.0f - fRatio;
    Real fTime = fOmRatio*m_fTMin + fRatio*m_fTMax;

    for (int i = 0; i < iIterations; i++)
    {
        Real fDifference = GetLength(m_fTMin,fTime) - fLength;
        if ( Math::FAbs(fDifference) < fTolerance )
            return fTime;

        fTime -= fDifference/GetSpeed(fTime);
    }

    // Newton's method failed.  If this happens, increase iterations or
    // tolerance or integration accuracy.
    return Math::MAX_REAL;
}
//----------------------------------------------------------------------------


