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

#include "MgcExtraSpin.h"
using namespace Mgc;

//----------------------------------------------------------------------------
ExtraSpin::ExtraSpin ()
    :
    m_kAxis(0.0f,0.0f,0.0f),
    m_kQuat(0.0f,0.0f,0.0f,0.0f)
{
    m_fAngle = 0.0f;
    m_iExtraSpins = 0;
    m_eInfo = NONE;
}
//----------------------------------------------------------------------------
void ExtraSpin::PreprocessKeys (int iNumKeys, ExtraSpin* akKey)
{
    // Determine interpolation type, compute extra spins, and adjust
    // angles accordingly.
    int i;
    for (i = 0; i < iNumKeys-1; i++)
    {
        ExtraSpin& rkKey0 = akKey[i];
        ExtraSpin& rkKey1 = akKey[i+1];
        Real fDiff = rkKey1.m_fAngle - rkKey0.m_fAngle;
        if ( Math::FAbs(fDiff) < Math::TWO_PI )
        {
            rkKey0.m_iExtraSpins = 0;
            rkKey0.m_eInfo = NONE;
        }
        else
        {
            rkKey0.m_iExtraSpins = int(fDiff/Math::TWO_PI);
            
            if ( rkKey0.m_kAxis == rkKey1.m_kAxis )
                rkKey0.m_eInfo = SAME_AXIS;
            else if ( rkKey0.m_fAngle != 0.0f )
                rkKey0.m_eInfo = DIFF_AXIS_NO_ZERO;
            else
                rkKey0.m_eInfo = DIFF_AXIS_ZERO;
        }
    }

    // Eliminate any non-acute angles between successive quaternions.  This
    // is done to prevent potential discontinuities that are the result of
    // invalid intermediate value quaternions.
    for (i = 0; i < iNumKeys-1; i++)
    {
        ExtraSpin& rkKey0 = akKey[i];
        ExtraSpin& rkKey1 = akKey[i+1];
        if ( rkKey0.m_kQuat.Dot(rkKey1.m_kQuat) < 0.0f )
            rkKey1.m_kQuat = -rkKey1.m_kQuat;
    }

    // Clamp identity quaternions so that |w| <= 1 (avoids problems with
    // call to acos in SlerpExtraSpins).
    for (i = 0; i < iNumKeys; i++)
    {
        ExtraSpin& rkKey = akKey[i];
        if ( rkKey.m_kQuat.w < -1.0f )
            rkKey.m_kQuat.w = -1.0f;
        else if ( rkKey.m_kQuat.w > 1.0f )
            rkKey.m_kQuat.w = 1.0f;
    }
}
//----------------------------------------------------------------------------
void ExtraSpin::Interpolate (Real fTime, const ExtraSpin& rkNextKey,
    ExtraSpin& rkInterpKey)
{
    // assert:  0 <= fTime <= 1

    switch ( m_eInfo )
    {
        case NONE:
        {
            rkInterpKey.m_kQuat = Quaternion::Slerp(fTime,m_kQuat,
                rkNextKey.m_kQuat);
            break;
        }
        case SAME_AXIS:
        {
            rkInterpKey.m_fAngle = (1.0f-fTime)*m_fAngle +
                fTime*rkNextKey.m_fAngle;
            rkInterpKey.m_kAxis = m_kAxis;
            rkInterpKey.m_kQuat.FromAngleAxis(rkInterpKey.m_fAngle,
                rkInterpKey.m_kAxis);
            break;
        }
        case DIFF_AXIS_NO_ZERO:
        {
            rkInterpKey.m_kQuat = Quaternion::SlerpExtraSpins(fTime,
                m_kQuat,rkNextKey.m_kQuat,m_iExtraSpins);
            break;
        }
        case DIFF_AXIS_ZERO:
        {
            rkInterpKey.m_fAngle = (1.0f-fTime)*m_fAngle +
                fTime*rkNextKey.m_fAngle;
            InterpolateAxis(fTime,m_kAxis,rkNextKey.m_kAxis,
                rkInterpKey.m_kAxis);
            rkInterpKey.m_kQuat.FromAngleAxis(rkInterpKey.m_fAngle,
                rkInterpKey.m_kAxis);
            break;
        }
    }
}
//----------------------------------------------------------------------------
void ExtraSpin::InterpolateAxis (Real fTime, const Vector3& rkAxis0,
    const Vector3& rkAxis1, Vector3& rkInterpAxis)
{
    // assert:  rkAxis0 and rkAxis1 are unit length
    // assert:  rkAxis0.Dot(rkAxis1) >= 0
    // assert:  0 <= fTime <= 1

    Real fCos = rkAxis0.Dot(rkAxis1);  // >= 0 by assertion
    if ( fCos > 1.0f ) // round-off error might create problems in acos call
        fCos = 1.0f;

    Real fAngle = Math::ACos(fCos);
    Real fInvSin = 1.0f/Math::Sin(fAngle);
    Real fTimeAngle = fTime*fAngle;
    Real fCoeff0 = Math::Sin(fAngle - fTimeAngle)*fInvSin;
    Real fCoeff1 = Math::Sin(fTimeAngle)*fInvSin;

    rkInterpAxis = fCoeff0*rkAxis0 + fCoeff1*rkAxis1;
}
//----------------------------------------------------------------------------


