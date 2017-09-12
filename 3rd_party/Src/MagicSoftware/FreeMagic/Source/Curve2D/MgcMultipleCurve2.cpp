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

#include "MgcMultipleCurve2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
MultipleCurve2::MultipleCurve2 (int iSegments, Real* afTime)
    :
    Curve2(afTime[0],afTime[iSegments])
{
    m_iSegments = iSegments;
    m_afTime = afTime;
    m_afLength = NULL;
    m_afAccumLength = NULL;
}
//----------------------------------------------------------------------------
MultipleCurve2::~MultipleCurve2 ()
{
    delete[] m_afTime;
    delete[] m_afLength;
    delete[] m_afAccumLength;
}
//----------------------------------------------------------------------------
int MultipleCurve2::GetSegments () const
{
    return m_iSegments;
}
//----------------------------------------------------------------------------
const Real* MultipleCurve2::GetTimes () const
{
    return m_afTime;
}
//----------------------------------------------------------------------------
void MultipleCurve2::GetKeyInfo (Real fTime, int& riKey, Real& rfDt) const
{
    if ( fTime <= m_afTime[0] )
    {
        riKey = 0;
        rfDt = 0.0f;
    }
    else if ( fTime >= m_afTime[m_iSegments] )
    {
        riKey = m_iSegments-1;
        rfDt = m_afTime[m_iSegments] - m_afTime[m_iSegments-1];
    }
    else
    {
        for (int i = 0; i < m_iSegments; i++)
        {
            if ( fTime < m_afTime[i+1] )
            {
                riKey = i;
                rfDt = fTime - m_afTime[i];
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------
Real MultipleCurve2::GetSpeedWithData (Real fTime, void* pvData)
{
    MultipleCurve2* pvThis = *(MultipleCurve2**) pvData;
    int iKey = *(int*)((char*)pvData + sizeof(pvThis));
    return pvThis->GetSpeedKey(iKey,fTime);
}
//----------------------------------------------------------------------------
void MultipleCurve2::InitializeLength () const
{
    m_afLength = new Real[m_iSegments];
    m_afAccumLength = new Real[m_iSegments];

    // arc lengths of the segments
    int iKey;
    for (iKey = 0; iKey < m_iSegments; iKey++)
    {
        m_afLength[iKey] = GetLengthKey(iKey,0.0f,
            m_afTime[iKey+1]-m_afTime[iKey]);
    }

    // accumulative arc length
    m_afAccumLength[0] = m_afLength[0];
    for (iKey = 1; iKey < m_iSegments; iKey++)
        m_afAccumLength[iKey] = m_afAccumLength[iKey-1] + m_afLength[iKey];
}
//----------------------------------------------------------------------------
Real MultipleCurve2::GetLength (Real fT0, Real fT1) const
{
    assert( m_fTMin <= fT0 && fT0 <= m_fTMax );
    assert( m_fTMin <= fT1 && fT1 <= m_fTMax );
    assert( fT0 <= fT1 );

    if ( !m_afLength )
        InitializeLength();

    int iKey0, iKey1;
    Real fDt0, fDt1;
    GetKeyInfo(fT0,iKey0,fDt0);
    GetKeyInfo(fT1,iKey1,fDt1);

    Real fLength;
    if ( iKey0 < iKey1 )
    {
        // accumulate full-segment lengths
        fLength = 0.0f;
        for (int i = iKey0+1; i < iKey1; i++)
            fLength += m_afLength[i];
        
        // add on partial first segment
        fLength += GetLengthKey(iKey0,fDt0,m_afTime[iKey0+1]-m_afTime[iKey0]);
        
        // add on partial last segment
        fLength += GetLengthKey(iKey1,0.0f,fDt1);
    }
    else
    {
        fLength = GetLengthKey(iKey0,fDt0,fDt1);
    }

    return fLength;
}
//----------------------------------------------------------------------------
Real MultipleCurve2::GetTime (Real fLength, int iIterations,
    Real fTolerance) const
{
    if ( !m_afLength )
        InitializeLength();

    if ( fLength <= 0.0f )
        return m_fTMin;

    if ( fLength >= m_afAccumLength[m_iSegments-1] )
        return m_fTMax;

    int iKey;
    for (iKey = 0; iKey < m_iSegments; iKey++)
    {
        if ( fLength < m_afAccumLength[iKey] )
            break;
    }
    if ( iKey >= m_iSegments )
        return m_afTime[m_iSegments];

    // try Newton's method first for rapid convergence
    Real fL0, fL1;
    if ( iKey == 0 )
    {
        fL0 = fLength;
        fL1 = m_afAccumLength[0];
    }
    else
    {
        fL0 = fLength - m_afAccumLength[iKey-1];
        fL1 = m_afAccumLength[iKey] - m_afAccumLength[iKey-1];
    }

    // use Newton's method to invert the arc length integral
    Real fDt1 = m_afTime[iKey+1] - m_afTime[iKey];
    Real fDt0 = fDt1*fL0/fL1;
    for (int i = 0; i < iIterations; i++)
    {
        Real fDifference = GetLengthKey(iKey,0.0f,fDt0) - fL0;
        if ( Math::FAbs(fDifference) <= fTolerance )
            return m_afTime[iKey] + fDt0;

        fDt0 -= fDifference/GetSpeedKey(iKey,fDt0);
    }

    // Newton's method failed.  If this happens, increase iterations or
    // tolerance or integration accuracy.
    return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
Real MultipleCurve2::GetVariation (Real fT0, Real fT1, const Vector2* pkP0,
    const Vector2* pkP1) const
{
    assert( m_fTMin <= fT0 && fT0 <= m_fTMax );
    assert( m_fTMin <= fT1 && fT1 <= m_fTMax );
    assert( fT0 <= fT1 );

    // construct line segment, A + (t-t0)*B
    Vector2 kP0, kP1;
    if ( !pkP0 )
    {
        kP0 = GetPosition(fT0);
        pkP0 = &kP0;
    }
    if ( !pkP1 )
    {
        kP1 = GetPosition(fT1);
        pkP1 = &kP1;
    }
    Real fInvDT = 1.0f/(fT1 - fT0);
    Vector2 kA, kB = fInvDT*(*pkP1 - *pkP0);

    int iKey0, iKey1;
    Real fDt0, fDt1;
    GetKeyInfo(fT0,iKey0,fDt0);
    GetKeyInfo(fT1,iKey1,fDt1);

    Real fVariation;
    if ( iKey0 < iKey1 )
    {
        // accumulate full-segment variations
        fVariation = 0.0f;
        for (int i = iKey0+1; i < iKey1; i++)
        {
            kA = *pkP0 + (m_afTime[i] - fT0)*kB;
            fVariation += GetVariationKey(i,0.0f,m_afTime[i+1]-m_afTime[i],
                kA,kB);
        }
        
        // add on partial first segment
        kA = *pkP0 + (m_afTime[iKey0] - fT0)*kB;
        fVariation += GetVariationKey(iKey0,fDt0,
            m_afTime[iKey0+1]-m_afTime[iKey0],kA,kB);
        
        // add on partial last segment
        kA = *pkP0 + (m_afTime[iKey1] - fT0)*kB;
        fVariation += GetVariationKey(iKey1,0.0f,fDt1,kA,kB);
    }
    else
    {
        kA = *pkP0 + (m_afTime[iKey0] - fT0)*kB;
        fVariation = GetVariationKey(iKey0,fDt0,fDt1,kA,kB);
    }

    return fVariation;
}
//----------------------------------------------------------------------------


