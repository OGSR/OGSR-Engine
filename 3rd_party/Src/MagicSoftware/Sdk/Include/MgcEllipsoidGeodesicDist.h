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

#ifndef MGCELLIPSOIDGEODESICDIST_H
#define MGCELLIPSOIDGEODESICDIST_H

#include "MgcVector3.h"

namespace Mgc {

// Ellipsoid is (x/a0)^2 + (y/a1)^2 + (z/a2)^2 = 1.
// Input:   axis[] = { a0, a1, a2 }
//          P, Q are points on the ellipsoid
// Output:  Geodesic distance between P and Q (distance of shortest
//          path on ellipsoid between P and Q)

MAGICFM Real EllipsoidGeodesicDist (Real afAxis[3], const Vector3& rkP,
    const Vector3& rkQ);

} // namespace Mgc

#endif


