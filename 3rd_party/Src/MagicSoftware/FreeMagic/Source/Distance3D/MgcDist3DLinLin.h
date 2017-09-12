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

#ifndef MGCDIST3DLINLIN_H
#define MGCDIST3DLINLIN_H

#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"

namespace Mgc {

// squared distance measurements

MAGICFM Real SqrDistance (const Line3& rkLine0, const Line3& rkLine1,
    Real* pfLinP0 = NULL, Real* pfLinP1 = NULL);

MAGICFM Real SqrDistance (const Line3& rkLine, const Ray3& rkRay,
    Real* pfLinP = NULL, Real* pfRayP = NULL);

MAGICFM Real SqrDistance (const Line3& rkLine, const Segment3& rkSeg,
    Real* pfLinP = NULL, Real* pfSegP = NULL);

MAGICFM Real SqrDistance (const Ray3& rkRay0, const Ray3& rkRay1,
    Real* pfRayP0 = NULL, Real* pfRayP1 = NULL);

MAGICFM Real SqrDistance (const Ray3& rkRay, const Segment3& rkSeg,
    Real* pfRayP = NULL, Real* pfSegP = NULL);

MAGICFM Real SqrDistance (const Segment3& rkSeg0, const Segment3& rkSeg1,
    Real* pfSegP0 = NULL, Real* pfSegP1 = NULL);


// distance measurements

MAGICFM Real Distance (const Line3& rkLine0, const Line3& rkLine1,
    Real* pfLinP0 = NULL, Real* pfLinP1 = NULL);

MAGICFM Real Distance (const Line3& rkLine, const Ray3& rkRay,
    Real* pfLinP = NULL, Real* pfRayP = NULL);

MAGICFM Real Distance (const Line3& rkLine, const Segment3& rkSeg,
    Real* pfLinP = NULL, Real* pfSegP = NULL);

MAGICFM Real Distance (const Ray3& rkRay0, const Ray3& rkRay1,
    Real* pfRayP0 = NULL, Real* pfRayP1 = NULL);

MAGICFM Real Distance (const Ray3& rkRay, const Segment3& rkSeg,
    Real* pfRayP = NULL, Real* pfSegP = NULL);

MAGICFM Real Distance (const Segment3& rkSeg0, const Segment3& rkSeg1,
    Real* pfSegP0 = NULL, Real* pfSegP1 = NULL);

} // namespace Mgc

#endif


