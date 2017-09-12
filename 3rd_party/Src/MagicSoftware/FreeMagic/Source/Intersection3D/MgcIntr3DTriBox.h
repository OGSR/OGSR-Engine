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

#ifndef MGCINTR3DTRIBOX_H
#define MGCINTR3DTRIBOX_H

#include "MgcBox3.h"

namespace Mgc {

// triangle and box are stationary
MAGICFM bool TestIntersection (const Vector3* apkTri[3], const Box3& rkBox);

// triangle and box have constant linear velocity
MAGICFM bool TestIntersection (const Vector3* apkTri[3],
    const Vector3& rkTriVel, const Box3& rkBox, const Vector3& rkBoxVel,
    Real fTMax, Real& rfTFirst, Real& rfTLast);

} // namespace Mgc

#endif

