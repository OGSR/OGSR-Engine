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

#ifndef POSSPLINE_H
#define POSSPLINE_H

// Kochanek-Bartels tension-continuity-bias spline interpolation for
// positional data.

#include "MgcVector3.h"

class PosKey
{
public:
    PosKey ()
        :
        m_kP(Mgc::Vector3::ZERO)
    {
        m_fTime = 0.0f;
        m_fTension = 0.0f;
        m_fContinuity = 0.0f;
        m_fBias = 0.0f;
    }

    float& Time () { return m_fTime; }
    Mgc::Vector3& P () { return m_kP; }
    float& Tension () { return m_fTension; }
    float& Continuity () { return m_fContinuity; }
    float& Bias () { return m_fBias; }

private:
    float m_fTime;
    Mgc::Vector3 m_kP;

    // parameters are each in [-1,1]
    float m_fTension, m_fContinuity, m_fBias;
};

class PosSpline
{
public:
    // The keys should be sorted by increasing time.
    PosSpline (int iNumKeys, PosKey* akKey);
    ~PosSpline ();

    // The interpolators clamp the time to the range of times in the N input
    // keys, [key[0].time,key[N-1].time].
    Mgc::Vector3 Position (float fTime);      // X(t)
    Mgc::Vector3 Velocity (float fTime);      // X'(t)
    Mgc::Vector3 Acceleration (float fTime);  // X"(t)

    // length of the spline
    float Length (float fTime);
    float TotalLength ();

    // Evaluate position and derivatives by specifying arc length s along the
    // spline.  If L is the total length of the curve, then 0 <= s <= L is
    // required.
    Mgc::Vector3 PositionAL (float fS);
    Mgc::Vector3 VelocityAL (float fS);
    Mgc::Vector3 AccelerationAL (float fS);

private:
    class Poly
    {
    public:
        Mgc::Vector3 Position (float fU);      // P(u)
        Mgc::Vector3 Velocity (float fU);      // P'(u)
        Mgc::Vector3 Acceleration (float fU);  // P"(u)
        float Speed (float fU);
        float Length (float fU);

        // Time interval on which polynomial is valid, tmin <= t <= tmax.
        // The normalized time is u = (t - tmin)/(tmax - tmin).  The inverse
        // range 1/(tmax-tmin) is computed once and stored to avoid having to
        // use divisions during interpolation.
        float m_fTMin, m_fTMax, m_fTInvRange;

        // P(u) = C0 + u*C1 + u^2*C2 + u^3*C3,  0 <= u <= 1
        Mgc::Vector3 m_akC[4];

        // Legendre polynomial degree 5 for numerical integration
        static float ms_afModRoot[5];
        static float ms_afModCoeff[5];
    };

    void DoPolyLookup (float fTime, int& riI, float& rfU);

    int m_iNumPolys;
    Poly* m_akPoly;

    // support for arc length parameterization of spline
    void InvertIntegral (float fS, int& riI, float& rfU);
    float* m_afLength;
    float m_fTotalLength;
};

#endif


