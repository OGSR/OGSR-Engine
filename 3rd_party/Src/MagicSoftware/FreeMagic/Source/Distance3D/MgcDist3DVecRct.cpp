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

#include "MgcDist3DVecRct.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Rectangle3& rkRct,
    Real* pfSParam, Real* pfTParam)
{
    Vector3 kDiff = rkRct.Origin() - rkPoint;
    Real fA00 = rkRct.Edge0().SquaredLength();
    Real fA11 = rkRct.Edge1().SquaredLength();
    Real fB0 = kDiff.Dot(rkRct.Edge0());
    Real fB1 = kDiff.Dot(rkRct.Edge1());
    Real fS = -fB0, fT = -fB1;

    Real fSqrDist = kDiff.SquaredLength();

    if ( fS < 0.0f )
    {
        fS = 0.0f;
    }
    else if ( fS <= fA00 )
    {
        fS /= fA00;
        fSqrDist += fB0*fS;
    }
    else
    {
        fS = 1.0f;
        fSqrDist += fA00 + 2.0f*fB0;
    }

    if ( fT < 0.0f )
    {
        fT = 0.0f;
    }
    else if ( fT <= fA11 )
    {
        fT /= fA11;
        fSqrDist += fB1*fT;
    }
    else
    {
        fT = 1.0f;
        fSqrDist += fA11 + 2.0f*fB1;
    }

    if ( pfSParam )
        *pfSParam = fS;

    if ( pfTParam )
        *pfTParam = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Rectangle3& rkRct,
    Real* pfSParam, Real* pfTParam)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkRct,pfSParam,pfTParam));
}
//----------------------------------------------------------------------------


