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

#include "MgcDist3DLinTri.h"
#include "MgcDist3DLinPgm.h"
#include "MgcDist3DTriPgm.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Triangle3& rkTri, const Parallelogram3& rkPgm,
    Real* pfTriP0, Real* pfTriP1, Real* pfPgmP0, Real* pfPgmP1)
{
    Real fS, fT, fS0, fT0;  // triangle parameters
    Real fU, fV, fU0, fV0;  // parallelogram parameters
    Real fSqrDist, fSqrDist0;
    Segment3 kSeg;

    // compare edges of tri against all of pgm
    kSeg.Origin() = rkTri.Origin();
    kSeg.Direction() = rkTri.Edge0();
    fSqrDist = SqrDistance(kSeg,rkPgm,&fS,&fU,&fV);
    fT = 0.0f;

    kSeg.Direction() = rkTri.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkPgm,&fT0,&fU0,&fV0);
    fS0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = kSeg.Origin() + rkTri.Edge0();
    kSeg.Direction() = kSeg.Direction() - rkTri.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkPgm,&fT0,&fU0,&fV0);
    fS0 = 1.0f-fT0;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    // compare edges of pgm against all of tri
    kSeg.Origin() = rkPgm.Origin();
    kSeg.Direction() = rkPgm.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkTri,&fU0,&fS0,&fT0);
    fV0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Direction() = rkPgm.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkTri,&fV0,&fS0,&fT0);
    fU0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm.Origin() + rkPgm.Edge1();
    kSeg.Direction() = rkPgm.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkTri,&fU0,&fS0,&fT0);
    fV0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = rkPgm.Origin() + rkPgm.Edge0();
    kSeg.Direction() = rkPgm.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkTri,&fV0,&fS0,&fT0);
    fU0 = 1.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    if ( pfTriP0 )
        *pfTriP0 = fS;

    if ( pfTriP1 )
        *pfTriP1 = fT;

    if ( pfPgmP0 )
        *pfPgmP0 = fU;

    if ( pfPgmP1 )
        *pfPgmP1 = fV;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Triangle3& rkTri, const Parallelogram3& rkPgm,
    Real* pfTriP0, Real* pfTriP1, Real* pfPgmP0, Real* pfPgmP1)
{
    return Math::Sqrt(SqrDistance(rkTri,rkPgm,pfTriP0,pfTriP1,pfPgmP0,
        pfPgmP1));
}
//----------------------------------------------------------------------------


