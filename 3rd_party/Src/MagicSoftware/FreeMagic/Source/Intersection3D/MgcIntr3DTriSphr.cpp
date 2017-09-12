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

#include "MgcDist3DVecTri.h"
#include "MgcIntr3DTriSphr.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Triangle3& rkT, const Sphere& rkS)
{
    Real fSqrDist = SqrDistance(rkS.Center(),rkT);
    Real fRSqr = rkS.Radius()*rkS.Radius();
    return fSqrDist < fRSqr;
}
//----------------------------------------------------------------------------

