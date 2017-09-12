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

#ifndef MGCDIST3DLINCIR_H
#define MGCDIST3DLINCIR_H

#include "MgcCircle3.h"
#include "MgcLine3.h"

namespace Mgc {

MAGICFM Real SqrDistance (const Line3& rkLine, const Circle3& rkCircle,
    Vector3* pkLineClosest = NULL, Vector3* pkCircleClosest = NULL);

MAGICFM Real Distance (const Line3& rkLine, const Circle3& rkCircle,
    Vector3* pkLineClosest = NULL, Vector3* pkCircleClosest = NULL);

} // namespace Mgc

#endif


