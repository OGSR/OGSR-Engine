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

#include "MgcDist3DLinPgm.h"
#include "MgcDist3DPgmPgm.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Parallelogram3& rkPgm0,
    const Parallelogram3& rkPgm1, Real* pfPgm0P0, Real* pfPgm0P1,
    Real* pfPgm1P0, Real* pfPgm1P1)
{
    Real fS, fT, fS0, fT0;  // pgm0 parameters
    Real fU, fV, fU0, fV0;  // pgm1 parameters
    Real fSqrDist, fSqrDist0;
    Segment3 kSeg;

    // compare edges of pgm0 against all of pgm1
    kSeg.Origin() = rkPgm0.Origin();
    kSeg.Direction() = rkPgm0.Edge0();
    fSqrDist = SqrDistance(kSeg,rkPgm1,&fS,&fU,&fV);
    fT = 0.0f;

    kSeg.Direction() = rkPgm0.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkPgm1,&fT0,&fU0,&fV0);
    fS0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm0.Origin() + rkPgm0.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm1,&fT0,&fU0,&fV0);
    fS0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm0.Origin() + rkPgm0.Edge1();
    kSeg.Direction() = rkPgm0.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm1,&fS0,&fU0,&fV0);
    fT0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    // compare edges of pgm1 against all of pgm0
    kSeg.Origin() = rkPgm1.Origin();
    kSeg.Direction() = rkPgm1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm0,&fU0,&fS0,&fT0);
    fV0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Direction() = rkPgm1.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkPgm0,&fV0,&fS0,&fT0);
    fU0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm1.Origin() + rkPgm1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm0,&fV0,&fS0,&fT0);
    fU0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm1.Origin() + rkPgm1.Edge1();
    kSeg.Direction() = rkPgm1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm0,&fU0,&fS0,&fT0);
    fV0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    if ( pfPgm0P0 )
        *pfPgm0P0 = fS;

    if ( pfPgm0P1 )
        *pfPgm0P1 = fT;

    if ( pfPgm1P0 )
        *pfPgm1P0 = fU;

    if ( pfPgm1P1 )
        *pfPgm1P1 = fV;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Parallelogram3& rkPgm0,
    const Parallelogram3& rkPgm1, Real* pfPgm0P0, Real* pfPgm0P1,
    Real* pfPgm1P0, Real* pfPgm1P1)
{
    return Math::Sqrt(SqrDistance(rkPgm0,rkPgm1,pfPgm0P0,pfPgm0P1,pfPgm1P0,
        pfPgm1P1));
}
//----------------------------------------------------------------------------


