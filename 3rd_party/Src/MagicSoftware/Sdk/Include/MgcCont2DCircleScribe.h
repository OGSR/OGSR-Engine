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

#ifndef MGCCONT2DCIRCLESCRIBE_H
#define MGCCONT2DCIRCLESCRIBE_H

#include "MgcCircle2.h"

namespace Mgc {

// All functions return 'true' if circle has been constructed, 'false'
// otherwise (input points are linearly dependent).

// circle containing three 2D points
MAGICFM bool Circumscribe (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, Circle2& rkCircle);

// circle inscribing triangle of three 2D points
MAGICFM bool Inscribe (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, Circle2& rkCircle);

} // namespace

#endif


