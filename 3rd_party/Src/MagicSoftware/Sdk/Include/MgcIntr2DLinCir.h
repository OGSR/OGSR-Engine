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

#ifndef MGCINTR2DLINCIR_H
#define MGCINTR2DLINCIR_H

#include "MgcArc2.h"
#include "MgcCircle2.h"
#include "MgcLine2.h"
#include "MgcRay2.h"
#include "MgcSegment2.h"

namespace Mgc {

// The return value is 'true' if and only if the objects intersect.  The
// returned quantity is 0, 1, or 2 and is the number of valid points in
// akPoint[2] (the intersection points).

MAGICFM bool FindIntersection (const Line2& rkLine, const Circle2& rkCircle,
    int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Line2& rkLine, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Ray2& rkRay, const Circle2& rkCircle,
    int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Ray2& rkRay, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Segment2& rkSegment,
    const Circle2& rkCircle, int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Segment2& rkSegment, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2]);

} // namespace Mgc

#endif


