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

#ifndef MGCCONT3DCAPSULE_H
#define MGCCONT3DCAPSULE_H

#include "MgcCapsule.h"
#include "MgcSphere.h"

namespace Mgc {

// Compute axis of capsule segment using least-squares fit.  Radius is
// maximum distance from points to axis.  Hemispherical caps are chosen
// as close together as possible.
MAGICFM Capsule ContCapsule (int iQuantity, const Vector3* akPoint);

// This function allows for selection of vertices from a pool.  The
// return value is 'true' if and only if at least one vertex is valid.
MAGICFM bool ContCapsule (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Capsule& rkCapsule);

// Test for containment, point x is in capsule if
//    Sqr(Distance(x,capsule.segment)) <= Sqr(capsule.radius)
// If an epsilon e is supplied, the test becomes
//    Sqr(Distance(x,capsule.segment)) <= Sqr(capsule.radius) + e
MAGICFM bool InCapsule (const Vector3& rkPoint, const Capsule& rkCapsule,
    Real fEpsilon = 0.0f);

MAGICFM bool InCapsule (const Sphere& rkSphere, const Capsule& rkCapsule);
MAGICFM bool InCapsule (const Capsule& rkTestCapsule,
    const Capsule& rkCapsule);

MAGICFM Capsule MergeCapsules (const Capsule& rkCapsule0,
    const Capsule& rkCapsule1);

} // namespace Mgc

#endif


