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

#include "MgcTorus.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Torus::Torus (Real fRo, Real fRi)
{
    assert( fRo > 0.0f && fRi > 0.0f );

    m_fRo = fRo;
    m_fRi = fRi;
}
//----------------------------------------------------------------------------
Vector3 Torus::Position (Real fS, Real fT)
{
    Real fTwoPiS = Math::TWO_PI*fS;
    Real fTwoPiT = Math::TWO_PI*fT;
    Real fCosTwoPiS = Math::Cos(fTwoPiS);
    Real fSinTwoPiS = Math::Sin(fTwoPiS);
    Real fCosTwoPiT = Math::Cos(fTwoPiT);
    Real fSinTwoPiT = Math::Sin(fTwoPiT);
    Real fRc = m_fRo + m_fRi*fCosTwoPiT;
    Vector3 kPos(fRc*fCosTwoPiS,fRc*fSinTwoPiS,m_fRi*fSinTwoPiT);
    return kPos;
}
//----------------------------------------------------------------------------
Vector3 Torus::Normal (Real fS, Real fT)
{
    Real fTwoPiS = Math::TWO_PI*fS;
    Real fCosTwoPiS = Math::Cos(fTwoPiS);
    Real fSinTwoPiS = Math::Sin(fTwoPiS);
    Vector3 kPos = Position(fS,fT);
    Vector3 kNor(kPos.x-m_fRo*fCosTwoPiS,kPos.y-m_fRo*fSinTwoPiS,kPos.z);
    kNor.Unitize();
    return kNor;
}
//----------------------------------------------------------------------------
void Torus::GetParameters (const Vector3& rkPos, Real& rfS,
    Real& rfT) const
{
    const Real fEpsilon = 1e-06f;

    Real fRc = Math::Sqrt(rkPos.x*rkPos.x + rkPos.y*rkPos.y);
    Real fAngle;

    if ( fRc < fEpsilon )
    {
        rfS = 0.0f;
    }
    else
    {
        fAngle = Math::ATan2(rkPos.y,rkPos.x);
        if ( fAngle >= 0.0f )
            rfS = fAngle*Math::INV_TWO_PI;
        else
            rfS = 1.0f + fAngle*Math::INV_TWO_PI;
    }

    Real fDiff = fRc - m_fRo;
    if ( Math::FAbs(fDiff) < fEpsilon && Math::FAbs(rkPos.z) < fEpsilon )
    {
        rfT = 0.0f;
    }
    else
    {
        fAngle = Math::ATan2(rkPos.z,fDiff);
        if ( fAngle >= 0.0f )
            rfT = fAngle*Math::INV_TWO_PI;
        else
            rfT = 1.0f + fAngle*Math::INV_TWO_PI;
    }
}
//----------------------------------------------------------------------------


