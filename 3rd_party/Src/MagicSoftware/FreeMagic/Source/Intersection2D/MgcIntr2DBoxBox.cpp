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

#include "MgcIntr2DBoxBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Box2& rkBox0, const Box2& rkBox1)
{
    // convenience variables
    const Vector2* akA = rkBox0.Axes();
    const Vector2* akB = rkBox1.Axes();
    const Real* afEA = rkBox0.Extents();
    const Real* afEB = rkBox1.Extents();

    // compute difference of box centers, D = C1-C0
    Vector2 kD = rkBox1.Center() - rkBox0.Center();

    Real aafAbsAdB[2][2], fAbsAdD, fRSum;
    
    // axis C0+t*A0
    aafAbsAdB[0][0] = Math::FAbs(akA[0].Dot(akB[0]));
    aafAbsAdB[0][1] = Math::FAbs(akA[0].Dot(akB[1]));
    fAbsAdD = Math::FAbs(akA[0].Dot(kD));
    fRSum = afEA[0] + afEB[0]*aafAbsAdB[0][0] + afEB[1]*aafAbsAdB[0][1];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*A1
    aafAbsAdB[1][0] = Math::FAbs(akA[1].Dot(akB[0]));
    aafAbsAdB[1][1] = Math::FAbs(akA[1].Dot(akB[1]));
    fAbsAdD = Math::FAbs(akA[1].Dot(kD));
    fRSum = afEA[1] + afEB[0]*aafAbsAdB[1][0] + afEB[1]*aafAbsAdB[1][1];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*B0
    fAbsAdD = Math::FAbs(akB[0].Dot(kD));
    fRSum = afEB[0] + afEA[0]*aafAbsAdB[0][0] + afEA[1]*aafAbsAdB[1][0];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*B1
    fAbsAdD = Math::FAbs(akB[1].Dot(kD));
    fRSum = afEB[1] + afEA[0]*aafAbsAdB[0][1] + afEA[1]*aafAbsAdB[1][1];
    if ( fAbsAdD > fRSum )
        return false;

    return true;
}
//----------------------------------------------------------------------------


