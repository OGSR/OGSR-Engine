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
#include "MgcIntr3DLinLoz.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment,
    const Lozenge& rkLozenge)
{
    Real fSqrDist = SqrDistance(rkSegment,rkLozenge.Rectangle());
    return fSqrDist <= rkLozenge.Radius()*rkLozenge.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Lozenge& rkLozenge)
{
    Real fSqrDist = SqrDistance(rkRay,rkLozenge.Rectangle());
    return fSqrDist <= rkLozenge.Radius()*rkLozenge.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Lozenge& rkLozenge)
{
    Real fSqrDist = SqrDistance(rkLine,rkLozenge.Rectangle());
    return fSqrDist <= rkLozenge.Radius()*rkLozenge.Radius();
}
//----------------------------------------------------------------------------


