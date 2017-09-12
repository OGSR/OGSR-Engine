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

#ifndef MGCINTR3DSPHRSPHR_H
#define MGCINTR3DSPHRSPHR_H

#include "MgcSphere.h"

namespace Mgc {

// Test for intersection of static spheres.
MAGICFM bool TestIntersection (const Sphere& rkS0, const Sphere& rkS1);

// Test for intersection of moving spheres.
MAGICFM bool TestIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Real fTime, const Vector3& rkV0, const Vector3& rkV1);

// Find the intersection of static spheres.  The circle of intersection is
// X(t) = C + R*(cos(t)*U + sin(t)*V) for 0 <= t < 2*pi.
MAGICFM bool FindIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Vector3& rkU, Vector3& rkV, Vector3& rkC, Real& rfR);

// Find the first time/point of contact of moving spheres.  If the spheres
// are initially intersecting, the reported first time is 0 and a point of
// contact is *estimated* as the average of sphere centers.
MAGICFM bool FindIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Real fTime, const Vector3& rkV0, const Vector3& rkV1, Real& rfFirstTime,
    Vector3& rkFirstPoint);

} // namespace Mgc

#endif

