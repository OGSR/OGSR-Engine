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

#ifndef MGCINTR2DLINLIN_H
#define MGCINTR2DLINLIN_H

#include "MgcLine2.h"
#include "MgcRay2.h"
#include "MgcSegment2.h"

namespace Mgc {

// The return value is 'true' if and only if the objects intersect.  If an
// intersection occurs, the returned values are the parameter values for the
// point(s) of intersection with respect to the first object parameterized
// by P+t*D (t in [-inf,+inf] for line, t in [0,+inf] for ray, t in [0,1]
// for segment).  The returned quantity is 1 or 2.  If 2, then the returned
// values are the end points of an interval of intersection.

MAGICFM bool FindIntersection (const Line2& rkLine0, const Line2& rkLine1,
    int& riQuantity, Real afT[2]);

MAGICFM bool FindIntersection (const Line2& rkLine, const Ray2& rkRay,
    int& riQuantity, Real afT[2]);

MAGICFM bool FindIntersection (const Line2& rkLine, const Segment2& rkSegment,
    int& riQuantity, Real afT[2]);

MAGICFM bool FindIntersection (const Ray2& rkRay0, const Ray2& rkRay1,
    int& riQuantity, Real afT[2]);

MAGICFM bool FindIntersection (const Ray2& rkRay, const Segment2& rkSegment,
    int& riQuantity, Real afT[2]);

MAGICFM bool FindIntersection (const Segment2& rkSegment0,
    const Segment2& rkSegment1, int& riQuantity, Real afT[2]);

} // namespace Mgc

#endif


