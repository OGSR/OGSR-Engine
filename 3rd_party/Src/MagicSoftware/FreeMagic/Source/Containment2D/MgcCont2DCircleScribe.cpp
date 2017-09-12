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

#include "MgcCont2DCircleScribe.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::Circumscribe (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, Circle2& rkCircle)
{
    Vector2 kE10 = rkV1 - rkV0;
    Vector2 kE20 = rkV2 - rkV0;

    Real aafA[2][2] =
    {
        kE10.x, kE10.y,
        kE20.x, kE20.y
    };

    Real afB[2] =
    {
        0.5f*kE10.SquaredLength(),
        0.5f*kE20.SquaredLength()
    };

    Vector2 kSol;
    if ( LinearSystem::Solve2(aafA,afB,(Real*)&kSol) )
    {
        rkCircle.Center() = rkV0 + kSol;
        rkCircle.Radius() = kSol.Length();
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------
bool Mgc::Inscribe (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, Circle2& rkCircle)
{
    const Real fTolerance = 1e-06f;

    // edges
    Vector2 kE0 = rkV1 - rkV0;
    Vector2 kE1 = rkV2 - rkV1;
    Vector2 kE2 = rkV0 - rkV2;

    // normals
    Vector2 kN0 = kE0.UnitCross();
    Vector2 kN1 = kE1.UnitCross();
    Vector2 kN2 = kE2.UnitCross();

    Real fA0 = kN1.Dot(kE0);
    if ( Math::FAbs(fA0) < fTolerance )
        return false;

    Real fA1 = kN2.Dot(kE1);
    if ( Math::FAbs(fA1) < fTolerance )
        return false;

    Real fA2 = kN0.Dot(kE2);
    if ( Math::FAbs(fA2) < fTolerance )
        return false;

    Real fInvA0 = 1.0f/fA0;
    Real fInvA1 = 1.0f/fA1;
    Real fInvA2 = 1.0f/fA2;

    rkCircle.Radius() = 1.0f/(fInvA0 + fInvA1 + fInvA2);
    rkCircle.Center() = rkCircle.Radius()*(fInvA0*rkV0 + fInvA1*rkV1 +
        fInvA2*rkV2);

    return true;
}
//----------------------------------------------------------------------------


