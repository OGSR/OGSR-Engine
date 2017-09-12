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

#ifndef MGCDIST3DVECTRI_H
#define MGCDIST3DVECTRI_H

#include "MgcTriangle3.h"

namespace Mgc {

// squared distance measurements
MAGICFM Real SqrDistance (const Vector3& rkPoint, const Triangle3& rkTri,
    Real* pfSParam = NULL, Real* pfTParam = NULL);

// distance measurements
MAGICFM Real Distance (const Vector3& rkPoint, const Triangle3& rkTri,
    Real* pfSParam = NULL, Real* pfTParam = NULL);

} // namespace Mgc

#endif


