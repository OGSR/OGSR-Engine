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

#ifndef MGCDIST3DCIRCIR_H
#define MGCDIST3DCIRCIR_H

#include "MgcCircle3.h"

// Compute the distance between two circles.  In many configurations, there
// is exactly one pair of points that attains minimal distance.  However,
// there are configurations for which two pair of points attain minimal
// distance, for example when the circles are not coplanar and share the same
// center or when the circles are not coplanar yet parallel and their
// projections onto a single parallel plane intersect in two points.  Finally,
// when the two circles are coplanar and share the same center, there are
// infinitely many pairs of points that are equidistant with minimum distance.

namespace Mgc {

MAGICFM Real SqrDistance (const Circle3& rkCircle0, const Circle3& rkCircle1,
    Vector3* pkClosest0 = NULL, Vector3* pkClosest1 = NULL);

MAGICFM Real Distance (const Circle3& rkCircle0, const Circle3& rkCircle1,
    Vector3* pkClosest0 = NULL, Vector3* pkClosest1 = NULL);

} // namespace Mgc

#endif


