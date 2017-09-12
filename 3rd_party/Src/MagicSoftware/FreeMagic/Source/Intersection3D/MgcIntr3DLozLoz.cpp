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

#include "MgcDist3DRctRct.h"
#include "MgcIntr3DLozLoz.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Lozenge& rkL0, const Lozenge& rkL1)
{
    // TO DO.  Expose this value to the caller.
    const Real fEpsilon = 1e-06f;

    Real fSqrDist = SqrDistance(rkL0.Rectangle(),rkL1.Rectangle());
    Real fRSum = rkL0.Radius() + rkL1.Radius();
    Real fRSumSqr = fRSum*fRSum;
    return fSqrDist <= fRSumSqr + fEpsilon;
}
//----------------------------------------------------------------------------


