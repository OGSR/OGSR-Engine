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

#ifndef MGCINTR2DCIRBOX_H
#define MGCINTR2DCIRBOX_H

#include "MgcBox2.h"

namespace Mgc
{

// Find first time/point of contact of moving circle and oriented rectangle.
//
// Inputs are:
//   rkC:       circle center
//   fR:        circle radius
//   rkV:       circle velocity
//   rkBox:     oriented rectangle
//   rfTFirst:  first time of contact
//   rkIntr:    first point of contact
//
// Return -1 if initially intersecting, 0 if no intersection, +1 if intersects
// at some positive time.  First time/point of contact are only valid if the
// return value is +1.

MAGICFM int FindIntersection (const Vector2& rkC, Real fR, const Vector2& rkV,
    const Box2& rkBox, Real& rfTFirst, Vector2& rkIntr);

}

#endif
