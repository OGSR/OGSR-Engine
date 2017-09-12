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

#ifndef MGCAPPR2DLINEFIT_H
#define MGCAPPR2DLINEFIT_H

#include "MgcVector2.h"

namespace Mgc {

// Least-squares fit of a line to (x,f(x)) data by using distance
// measurements in the y-direction.  The resulting line is represented by
// y = A*x + B.  The return value is 'false' if the 2x2 coefficient matrix
// in the linear system that defines A and B is nearly singular.

MAGICFM bool HeightLineFit (int iQuantity, const Vector2* akPoint,
    Real& rfA, Real& rfB);


// Least-squares fit of a line to (x,y) data by using distance measurements
// orthogonal to the proposed line.  The resulting line is represented by
// Offset + t*Direction where the returned direction is a unit-length vector.

MAGICFM void OrthogonalLineFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkOffset, Vector2& rkDirection);


// This function allows for selection of vertices from a pool.  The return
// value is 'true' if and only if at least one vertex is valid.

MAGICFM bool OrthogonalLineFit (int iQuantity, const Vector2* akPoint,
    const bool* abValid, Vector2& rkOffset, Vector2& rkDirection);

} // namespace Mgc

#endif


