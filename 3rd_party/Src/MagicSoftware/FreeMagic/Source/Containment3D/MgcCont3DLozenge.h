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

#ifndef MGCCONT3DLOZENGE_H
#define MGCCONT3DLOZENGE_H

#include "MgcLozenge.h"

namespace Mgc {

// Compute plane of lozenge rectangle using least-squares fit.  Parallel
// planes are chosen close enough together so that all the data points lie
// between them.  The radius is half the distance between the two planes.
// The half-cylinder and quarter-cylinder side pieces are chosen using a
// method similar to that used for fitting by capsules.
MAGICFM Lozenge ContLozenge (int iQuantity, const Vector3* akPoint);

// This function allows for selection of vertices from a pool.  The
// return value is 'true' if and only if at least one vertex is valid.
MAGICFM bool ContLozenge (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Lozenge& rkLozenge);

// Test for containment, point x is in lozenge if
//    Sqr(Distance(x,lozenge.rectangle)) <= Sqr(lozenge.radius)
// If an epsilon e is supplied, the test becomes
//    Sqr(Distance(x,lozenge.rectangle)) <= Sqr(lozenge.radius) + e
MAGICFM bool InLozenge (const Vector3& rkPoint, const Lozenge& rkLozenge,
    Real fEpsilon = 0.0f);

} // namespace Mgc

#endif


