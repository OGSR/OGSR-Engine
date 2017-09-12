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

#ifndef MGCDIST3DVECQDR_H
#define MGCDIST3DVECQDR_H

// Given a quadric surface defined implicitly by
//   0 = C[0] + C[1]*X + C[2]*Y + C[3]*Z + C[4]*X^2 + C[5]*Y^2
//       + C[6]*Z^2 + C[7]*X*Y + C[8]*X*Z + C[9]*Y*Z
// find distance from point to the surface.

#include "MgcVector3.h"

namespace Mgc {

MAGICFM Real SqrDistance (const Vector3& rkPoint, const Real afQuad[10],
    Vector3& rkClosest);

MAGICFM Real Distance (const Vector3& rkPoint, const Real afQuad[10],
    Vector3& rkClosest);

} // namespace Mgc

#endif


