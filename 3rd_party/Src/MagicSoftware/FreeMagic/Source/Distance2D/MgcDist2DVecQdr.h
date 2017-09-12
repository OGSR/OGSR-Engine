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

#ifndef MGCDIST2DVECQDR_H
#define MGCDIST2DVECQDR_H

// Given a quadratic curve defined implicitly by
//   0 = C[0] + C[1]*X + C[2]*Y + C[3]*X^2 + C[4]*Y^2 + C[5]*X*Y
// find distance from point to the curve.

#include "MgcVector2.h"

namespace Mgc {

MAGICFM Real SqrDistance (const Vector2& rkPoint, const Real afQuad[6],
    Vector2& rkClosest);

MAGICFM Real Distance (const Vector2& rkPoint, const Real afQuad[6],
    Vector2& rkClosest);

} // namespace Mgc

#endif


