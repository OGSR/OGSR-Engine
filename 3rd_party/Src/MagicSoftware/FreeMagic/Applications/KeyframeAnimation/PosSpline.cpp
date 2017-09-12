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

#include "PosSpline.h"
using namespace Mgc;

// Legendre polynomial information for Gaussian quadrature of speed on domain
// [0,u], 0 <= u <= 1.  The polynomial is degree 5.
float PosSpline::Poly::ms_afModRoot[5] =
{
    // Legendre roots mapped to (root+1)/2
    0.046910077f,
    0.230765345f,
    0.5f,
    0.769234655f,
    0.953089922f
};

float PosSpline::Poly::ms_afModCoeff[5] =
{
    // original coefficients divided by 2
    0.118463442f,
    0.239314335f,
    0.284444444f,
    0.239314335f,
    0.118463442f
};

//----------------------------------------------------------------------------
PosSpline::PosSpline (int iNumKeys, PosKey* akKey)
{
    assert( iNumKeys >= 4 );

    m_iNumPolys = iNumKeys-3;
    m_akPoly = new Poly[m_iNumPolys];

    for (int i0=0,i1=1,i2=2,i3=3; i0<m_iNumPolys; i0++,i1++,i2++,i3++)
    {
        Vector3 kDiff10 = akKey[i1].P() - akKey[i0].P();
        Vector3 kDiff21 = akKey[i2].P() - akKey[i1].P();
        Vector3 kDiff32 = akKey[i3].P() - akKey[i2].P();

        // build multipliers at point P[i1]
        float fOmT0 = 1.0f - akKey[i1].Tension();
        float fOmC0 = 1.0f - akKey[i1].Continuity();
        float fOpC0 = 1.0f + akKey[i1].Continuity();
        float fOmB0 = 1.0f - akKey[i1].Bias();
        float fOpB0 = 1.0f + akKey[i1].Bias();
        float fAdj0 = 2.0f*(akKey[i2].Time() - akKey[i1].Time()) /
            (akKey[i2].Time() - akKey[i0].Time());
        float fOut0 = 0.5f*fAdj0*fOmT0*fOpC0*fOpB0;
        float fOut1 = 0.5f*fAdj0*fOmT0*fOmC0*fOmB0;

        // build outgoing tangent at P[i1]
        Vector3 kTOut = fOut1*kDiff21 + fOut0*kDiff10;

        // build multipliers at point P[i2]
        float fOmT1 = 1.0f - akKey[i2].Tension();
        float fOmC1 = 1.0f - akKey[i2].Continuity();
        float fOpC1 = 1.0f + akKey[i2].Continuity();
        float fOmB1 = 1.0f - akKey[i2].Bias();
        float fOpB1 = 1.0f + akKey[i2].Bias();
        float fAdj1 = 2.0f*(akKey[i2].Time() - akKey[i1].Time()) /
            (akKey[i3].Time() - akKey[i1].Time());
        float fIn0 = 0.5f*fAdj1*fOmT1*fOmC1*fOpB1;
        float fIn1 = 0.5f*fAdj1*fOmT1*fOpC1*fOmB1;

        // build incoming tangent at P[i2]
        Vector3 kTIn = fIn1*kDiff32 + fIn1*kDiff21;

        m_akPoly[i0].m_akC[0] = akKey[i1].P();
        m_akPoly[i0].m_akC[1] = kTOut;
        m_akPoly[i0].m_akC[2] = 3.0f*kDiff21 - 2.0f*kTOut - kTIn;
        m_akPoly[i0].m_akC[3] = -2.0f*kDiff21 + kTOut + kTIn;
        m_akPoly[i0].m_fTMin = akKey[i1].Time();
        m_akPoly[i0].m_fTMax = akKey[i2].Time();
        m_akPoly[i0].m_fTInvRange = 1.0f/(akKey[i2].Time()-akKey[i1].Time());
    }

    // compute arc lengths of polynomials and total length of spline
    m_afLength = new float[m_iNumPolys+1];
    m_afLength[0] = 0.0f;
    for (int i = 0; i < m_iNumPolys; i++)
    {
        // length of current polynomial
        float fPolyLength = m_akPoly[i].Length(1.0f);

        // total length of curve between poly[0] and poly[i+1]
        m_afLength[i+1] = m_afLength[i] + fPolyLength;
    }
    m_fTotalLength = m_afLength[m_iNumPolys];
}
//----------------------------------------------------------------------------
PosSpline::~PosSpline ()
{
    delete[] m_akPoly;
    delete[] m_afLength;
}
//----------------------------------------------------------------------------
void PosSpline::DoPolyLookup (float fTime, int& riI, float& rfU)
{
    // Lookup the polynomial that contains the input time in its domain of
    // evaluation.  Clamp to [tmin,tmax].

    if ( m_akPoly[0].m_fTMin < fTime )
    {
        if ( fTime < m_akPoly[m_iNumPolys-1].m_fTMax )
        {
            for (riI = 0; riI < m_iNumPolys; riI++)
            {
                if ( fTime < m_akPoly[riI].m_fTMax )
                    break;
            }
            rfU = (fTime-m_akPoly[riI].m_fTMin)*m_akPoly[riI].m_fTInvRange;
        }
        else
        {
            riI = m_iNumPolys-1;
            rfU = 1.0f;
        }
    }
    else
    {
        riI = 0;
        rfU = 0.0f;
    }
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Position (float fTime)
{
    int i;
    float fU;
    DoPolyLookup(fTime,i,fU);
    return m_akPoly[i].Position(fU);
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Velocity (float fTime)
{
    int i;
    float fU;
    DoPolyLookup(fTime,i,fU);
    return m_akPoly[i].Velocity(fU);
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Acceleration (float fTime)
{
    int i;
    float fU;
    DoPolyLookup(fTime,i,fU);
    return m_akPoly[i].Acceleration(fU);
}
//----------------------------------------------------------------------------
float PosSpline::Length (float fTime)
{
    int i;
    float fU;
    DoPolyLookup(fTime,i,fU);
    return m_akPoly[i].Length(fU);
}
//----------------------------------------------------------------------------
float PosSpline::TotalLength ()
{
    return m_fTotalLength;
}
//----------------------------------------------------------------------------
void PosSpline::InvertIntegral (float fS, int& riI, float& rfU)
{
    // clamp s to [0,L] so that t in [tmin,tmax]
    if ( fS <= 0.0f )
    {
        riI = 0;
        rfU = 0.0f;
        return;
    }

    if ( fS >= m_fTotalLength )
    {
        riI = m_iNumPolys-1;
        rfU = 1.0f;
        return;
    }

    // determine which polynomial corresponds to s
    float fDist;
    for (riI = 0; riI < m_iNumPolys; riI++)
    {
        if ( fS <= m_afLength[riI+1] )
        {
            // distance along segment
            fDist = fS - m_afLength[riI];

            // initial guess for inverting the arc length integral
            rfU = fDist/(m_afLength[riI+1]-m_afLength[riI]);
            break;
        }
    }

    // use Newton's method to invert the arc length integral
    const float fTolerance = 1e-06f;
    const int iMax = 32;
    for (int i = 0; i < iMax; i++)
    {
        float fDiff = m_akPoly[riI].Length(rfU) - fDist;
        if ( fabsf(fDiff) <= fTolerance )
            break;

        // assert: speed > 0
        rfU -= fDiff/m_akPoly[riI].Speed(rfU);
    }
}
//----------------------------------------------------------------------------
Vector3 PosSpline::PositionAL (float fS)
{
    int i;
    float fU;
    InvertIntegral(fS,i,fU);
    return m_akPoly[i].Position(fU);
}
//----------------------------------------------------------------------------
Vector3 PosSpline::VelocityAL (float fS)
{
    int i;
    float fU;
    InvertIntegral(fS,i,fU);
    return m_akPoly[i].Velocity(fU);
}
//----------------------------------------------------------------------------
Vector3 PosSpline::AccelerationAL (float fS)
{
    int i;
    float fU;
    InvertIntegral(fS,i,fU);
    return m_akPoly[i].Acceleration(fU);
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Poly::Position (float fU)
{
    Vector3 kResult = m_akC[0]+fU*(m_akC[1]+fU*(m_akC[2]+fU*m_akC[3]));
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Poly::Velocity (float fU)
{
    Vector3 kResult = m_akC[1]+fU*(2.0f*m_akC[2]+3.0f*fU*m_akC[3]);
    return kResult;
}
//----------------------------------------------------------------------------
Vector3 PosSpline::Poly::Acceleration (float fU)
{
    Vector3 kResult = 2.0f*m_akC[2]+6.0f*fU*m_akC[3];
    return kResult;
}
//----------------------------------------------------------------------------
float PosSpline::Poly::Speed (float fU)
{
    return Velocity(fU).Length();
}
//----------------------------------------------------------------------------
float PosSpline::Poly::Length (float fU)
{
    // Need to transform domain [0,u] to [-1,1].  If 0 <= x <= u
    // and -1 <= t <= 1, then x = u*(t+1)/2.
    float fResult = 0.0f;
    for (int i = 0; i < 5; i++)
        fResult += ms_afModCoeff[i]*Speed(fU*ms_afModRoot[i]);
    fResult *= fU;

    return fResult;
}
//----------------------------------------------------------------------------


