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

#ifndef MGCDIST3DVECCIR_H
#define MGCDIST3DVECCIR_H

#include "MgcCircle3.h"
#include "MgcDisk3.h"

namespace Mgc {

// Compute the distance from 'p' to 'circle'.  Returned value is distance.
// The closest circle point is returned as 'closest' if it is unique.  When
// 'p' is on the normal line C+t*N where C is circle center and N is the
// normal to plane containing circle, then all circle points are equidistant
// from 'p'.  In this case the returned point is (infinity,infinity,infinity).

MAGICFM Real SqrDistance (const Vector3& rkPoint, const Circle3& rkCircle,
    Vector3* pkClosest = NULL);

MAGICFM Real Distance (const Vector3& rkPoint, const Circle3& rkCircle,
    Vector3* pkClosest = NULL);

// Compute the distance from 'p' to 'disk' (circle plus its interior).
// Returned value is distance.  The closest disk point is returned as
// 'closest' (always unique).

MAGICFM Real SqrDistance (const Vector3& rkPoint, const Disk3& rkDisk,
    Vector3* pkClosest = NULL);

MAGICFM Real Distance (const Vector3& rkPoint, const Disk3& rkDisk,
    Vector3* pkClosest = NULL);

} // namespace Mgc

#endif


