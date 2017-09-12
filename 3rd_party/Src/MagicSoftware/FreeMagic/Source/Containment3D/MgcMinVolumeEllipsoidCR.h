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

#ifndef MGCMINVOLUMEELLIPSOIDCR_H
#define MGCMINVOLUMEELLIPSOIDCR_H

#include "MgcMatrix3.h"

namespace Mgc {

// The ellipsoid in general form is  X^t A X + B^t X + C = 0 where
// A is a positive definite 3x3 matrix, B is a 3x1 vector, C is a
// scalar, and X is a 3x1 vector.  Completing the square,
// (X-U)^t A (X-U) = U^t A U - C where U = -0.5 A^{-1} B.  Define
// M = A/(U^t A U - C).  The ellipsoid is (X-U)^t M (X-U) = 1.  Factor
// M = R^t D R where R is orthonormal and D is diagonal with positive
// diagonal terms.  If Y = R(X-U), then the ellipsoid is 1 = Y^t D Y =
// d1*y1^2+d2*y2^2+d3*y3^2.  For an ellipsoid (x/a)^2+(y/b)^2+(z/c)^2
// = 1, the volume is (4*pi/3)*a*b*c.  For Y^t D Y = 1, the volume is
// therefore (4*pi/3)/sqrt(d1*d2*d3).  Finally, note that det(M) =
// det(D) = d1*d2*d3, so the volume of the ellipsoid is
// (4*pi/3)/sqrt(det(M)).

// Compute minimal volume ellipsoid (X-C)^t R^t D R (X-C) = 1 given center
// C and orientation matrix R by finding diagonal D.  Minimal volume is
// (4*pi/3)/sqrt(D[0]*D[1]*D[2]).

MAGICFM void MinVolumeEllipsoidCR (int iQuantity, const Vector3* akPoint,
    const Vector3& rkC, const Matrix3& rkR, Real afD[3]);

} // namespace Mgc

#endif


