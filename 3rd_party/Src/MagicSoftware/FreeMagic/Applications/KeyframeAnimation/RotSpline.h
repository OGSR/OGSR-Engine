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

#ifndef ROTSPLINE_H
#define ROTSPLINE_H

// Kochanek-Bartels tension-continuity-bias spline interpolation adapted
// to quaternion interpolation.

#include "MgcQuaternion.h"

class RotKey
{
public:
    RotKey ()
        :
        m_kQ(Mgc::Quaternion::IDENTITY)
    {
        m_fTime = 0.0f;
        m_fTension = 0.0f;
        m_fContinuity = 0.0f;
        m_fBias = 0.0f;
    }

    float& Time () { return m_fTime; }
    Mgc::Quaternion& Q () { return m_kQ; }
    float& Tension () { return m_fTension; }
    float& Continuity () { return m_fContinuity; }
    float& Bias () { return m_fBias; }

private:
    float m_fTime;
    Mgc::Quaternion m_kQ;

    // parameters are each in [-1,1]
    float m_fTension, m_fContinuity, m_fBias;
};

class RotSpline
{
public:
    RotSpline (int iNumKeys, RotKey* akKey);
    ~RotSpline ();

    Mgc::Quaternion Q (float fTime);

private:
    class SquadPoly
    {
    public:
        Mgc::Quaternion Q (float fU);

        // Time interval on which polynomial is valid, tmin <= t <= tmax.
        // The normalized time is u = (t - tmin)/(tmax - tmin).  The inverse
        // range 1/(tmax-tmin) is computed once and stored to avoid having to
        // use divisions during interpolation.
        float m_fTMin, m_fTMax, m_fTInvRange;

        // Q(u) = Squad(2u(1-u),Slerp(u,p,q),Slerp(u,a,b))
        Mgc::Quaternion m_kP, m_kA, m_kB, m_kQ;
    };

    int m_iNumPolys;
    SquadPoly* m_akPoly;
};

#endif


