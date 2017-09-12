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

#ifndef MGCCONT3DSPHERE_H
#define MGCCONT3DSPHERE_H

#include "MgcSphere.h"

namespace Mgc {

MAGICFM Sphere ContSphereOfAABB (int iQuantity, const Vector3* akPoint);
MAGICFM Sphere ContSphereAverage (int iQuantity, const Vector3* akPoint);

// These two functions allow for selection of vertices from a pool.  The
// return value is 'true' if and only if at least one vertex is valid.
MAGICFM bool ContSphereOfAABB (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Sphere& rkSphere);
MAGICFM bool ContSphereAverage (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Sphere& rkSphere);

// Test for containment, point x is in sphere if
//    Sqr(Distance(x,sphere.center)) <= Sqr(sphere.radius)
// If an epsilon e is supplied, the test becomes
//    Sqr(Distance(x,sphere.center)) <= Sqr(sphere.radius) + e
MAGICFM bool InSphere (const Vector3& rkPoint, const Sphere& rkSphere,
    Real fEpsilon = 0.0f);

MAGICFM Sphere MergeSpheres (const Sphere& rkSphere0,
    const Sphere& rkSphere1);

} // namespace Mgc

#endif


