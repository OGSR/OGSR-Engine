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

#ifndef MGCCONT3DCIRCLESPHERESCRIBE_H
#define MGCCONT3DCIRCLESPHERESCRIBE_H

#include "MgcCircle3.h"
#include "MgcSphere.h"

namespace Mgc {

// All functions return 'true' if circle/sphere has been constructed,
// 'false' otherwise (input points are linearly dependent).

// circle containing three 3D points
MAGICFM bool Circumscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, Circle3& rkCircle);

// sphere containing four 3D points
MAGICFM bool Circumscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, Sphere& rkSphere);

// circle inscribing triangle of three 3D points
MAGICFM bool Inscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, Circle3& rkCircle);

// sphere inscribing tetrahedron of four 3D points
MAGICFM bool Inscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, Sphere& rkSphere);

} // namespace Mgc

#endif


