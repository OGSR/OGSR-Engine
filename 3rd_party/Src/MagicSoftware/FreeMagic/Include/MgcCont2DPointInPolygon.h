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

#ifndef MGCCONT2DPOINTINPOLYGON_H
#define MGCCONT2DPOINTINPOLYGON_H

// Given a polygon as an order list of vertices (x[i],y[i]) for
// 0 <= i < N and a test point (xt,yt), return 'true' if (xt,yt) is in
// the polygon and 'false' if it is not.  All queries require that the
// number of vertices satisfies N >= 3.

#include "MgcVector2.h"

namespace Mgc {

// general polygons
MAGICFM bool PointInPolygon (int iQuantity, const Vector2* akV,
    const Vector2& rkP);

// Algorithms for convex polygons.  The input polygons must have vertices in
// counterclockwise order.

// O(N) algorithm
MAGICFM bool PointInConvexOrderN (int iQuantity, const Vector2* akV,
    const Vector2& rkP);

// O(log N) algorithm, uses bisection and recursion
MAGICFM bool PointInConvexOrderLogN (int iQuantity, const Vector2* akV,
    const Vector2& rkP);

// O(log N) algorithm but hard-coded for the specified size.  The number at
// the end of the function name is the number of vertices in the convex
// polygon.
MAGICFM bool PointInConvex4 (const Vector2* akV, const Vector2& rkP);

} // namespace Mgc

#endif


