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

#include "MgcDist3DLinRct.h"
#include "MgcDist3DRctRct.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Rectangle3& rkRct0, const Rectangle3& rkRct1,
    Real* pfRct0P0, Real* pfRct0P1, Real* pfRct1P0, Real* pfRct1P1)
{
    Real fS, fT, fS0, fT0;  // rct0 parameters
    Real fU, fV, fU0, fV0;  // rct1 parameters
    Real fSqrDist, fSqrDist0;
    Segment3 kSeg;

    // compare edges of rct0 against all of rct1
    kSeg.Origin() = rkRct0.Origin();
    kSeg.Direction() = rkRct0.Edge0();
    fSqrDist = SqrDistance(kSeg,rkRct1,&fS,&fU,&fV);
    fT = 0.0f;

    kSeg.Direction() = rkRct0.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkRct1,&fT0,&fU0,&fV0);
    fS0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkRct0.Origin() + rkRct0.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkRct1,&fT0,&fU0,&fV0);
    fS0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkRct0.Origin() + rkRct0.Edge1();
    kSeg.Direction() = rkRct0.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkRct1,&fS0,&fU0,&fV0);
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
    kSeg.Origin() = rkRct1.Origin();
    kSeg.Direction() = rkRct1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkRct0,&fU0,&fS0,&fT0);
    fV0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Direction() = rkRct1.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkRct0,&fV0,&fS0,&fT0);
    fU0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkRct1.Origin() + rkRct1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkRct0,&fV0,&fS0,&fT0);
    fU0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkRct1.Origin() + rkRct1.Edge1();
    kSeg.Direction() = rkRct1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkRct0,&fU0,&fS0,&fT0);
    fV0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    if ( pfRct0P0 )
        *pfRct0P0 = fS;

    if ( pfRct0P1 )
        *pfRct0P1 = fT;

    if ( pfRct1P0 )
        *pfRct1P0 = fU;

    if ( pfRct1P1 )
        *pfRct1P1 = fV;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Rectangle3& rkRct0, const Rectangle3& rkRct1,
    Real* pfRct0P0, Real* pfRct0P1, Real* pfRct1P0, Real* pfRct1P1)
{
    return Math::Sqrt(SqrDistance(rkRct0,rkRct1,pfRct0P0,pfRct0P1,pfRct1P0,
        pfRct1P1));
}
//----------------------------------------------------------------------------


