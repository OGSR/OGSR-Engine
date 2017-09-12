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

#ifndef MGCAPPR3DELLIPSOIDFIT_H
#define MGCAPPR3DELLIPSOIDFIT_H

// The ellipsoid in general form is  X^t A X + B^t X + C = 0 where
// A is a positive definite 3x3 matrix, B is a 3x1 vector, C is a
// scalar, and X is a 3x1 vector.  Completing the square,
// (X-U)^t A (X-U) = U^t A U - C where U = -0.5 A^{-1} B.  Define
// M = A/(U^t A U - C).  The ellipsoid is (X-U)^t M (X-U) = 1.  Factor
// M = R^t D R where R is orthonormal and D is diagonal with positive
// diagonal terms.  The ellipsoid in factored form is
//     (X-U)^t R^t D^t R (X-U) = 1
//
// Find the least squares fit of a set of N points P[0] through P[N-1].
// Return value is the least-squares energy function at (U,R,D).

#include "MgcMatrix3.h"

namespace Mgc {

MAGICFM Real EllipsoidFit (int iQuantity, const Vector3* akPoint,
    Vector3& rkU, Matrix3& rkR, Real afD[3]);

} // namespace Mgc

#endif


