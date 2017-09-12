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

#include "MgcDist3DLinLin.h"
#include "MgcIntr3DCapCap.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Capsule& rkC0, const Capsule& rkC1)
{
    Real fSqrDist = SqrDistance(rkC0.Segment(),rkC1.Segment());
    Real fRSum = rkC0.Radius() + rkC1.Radius();
    Real fRSumSqr = fRSum*fRSum;

    return fSqrDist <= fRSumSqr;
}
//----------------------------------------------------------------------------


