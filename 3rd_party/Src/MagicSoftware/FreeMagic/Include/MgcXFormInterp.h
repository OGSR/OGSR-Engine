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

#ifndef MGCXFORMINTERP_H
#define MGCXFORMINTERP_H

// Interpolation of a pair of homogeneous transformations.  The upper left
// 3x3 block M represents rotations and scalings.  The upper right 3x1 block
// T represents translation.  The lower left 1x3 block is the zero vector
// (no perspective), and the lower right 1x1 block is scalar 1.
//
// Interpolate two homogeneous transforms <M0,T0> and <M1,T1> to obtain a
// homogeneous transform <M,T>.  Matrices M0 and M1 are factored using
// singular value decomposition.  The rotational components of the
// factorizations are interpolated using quaternions.  The scale components
// of the factorizations are interpolated using a geometric mean of the
// diagonal entries.  The translations are interpolated using linear
// interpolation.

#include "MgcQuaternion.h"

namespace Mgc {

MAGICFM void XFormInterp (
    const Matrix3& rkM0, const Vector3& rkT0,  // input
    const Matrix3& rkM1, const Vector3& rkT1,  // input
    Real fTime,  // input, 0 <= time <= 1
    Matrix3& rkM, Vector3& rkT);  // output

} // namespace Mgc

#endif


