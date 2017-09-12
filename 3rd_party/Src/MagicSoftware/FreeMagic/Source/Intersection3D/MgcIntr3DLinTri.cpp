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
#include "MgcIntr3DLinTri.h"
using namespace Mgc;

static const Real gs_fEpsilon = 1e-12f;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment,
    const Triangle3& rkTriangle)
{
    return SqrDistance(rkSegment,rkTriangle) <= gs_fEpsilon;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Triangle3& rkTriangle)
{
    return SqrDistance(rkRay,rkTriangle) <= gs_fEpsilon;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Triangle3& rkTriangle)
{
    return SqrDistance(rkLine,rkTriangle) <= gs_fEpsilon;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment,
    const Triangle3& rkTriangle, Vector3& rkPoint)
{
    Real fSegP;
    if ( SqrDistance(rkSegment,rkTriangle,&fSegP) <= gs_fEpsilon )
    {
        rkPoint = rkSegment.Origin() + fSegP*rkSegment.Direction();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray3& rkRay, const Triangle3& rkTriangle,
    Vector3& rkPoint)
{
    Real fRayP;
    if ( SqrDistance(rkRay,rkTriangle,&fRayP) <= gs_fEpsilon )
    {
        rkPoint = rkRay.Origin() + fRayP*rkRay.Direction();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line3& rkLine, const Triangle3& rkTriangle,
    Vector3& rkPoint)
{
    Real fLinP;
    if ( SqrDistance(rkLine,rkTriangle,&fLinP) <= gs_fEpsilon )
    {
        rkPoint = rkLine.Origin() + fLinP*rkLine.Direction();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------


