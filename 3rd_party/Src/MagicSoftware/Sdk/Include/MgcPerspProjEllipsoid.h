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

#ifndef MGCPERSPPROJELLIPSOID_H
#define MGCPERSPPROJELLIPSOID_H

#include "MgcMatrix2.h"
#include "MgcMatrix3.h"
#include "MgcPlane.h"

namespace Mgc {


class MAGICFM GeneralEllipsoid
{
public:
    // Ellipsoid is X^T A X + B^T X + C = 0 where A is symmetric 3x3, B and
    // X are 3x1, and C is a scalar.  Since A is symmetric, it is not
    // necessary to store all 9 values, but I use it for convenience of doing
    // matrix/vector arithmetic.
    Matrix3 m_kA;
    Vector3 m_kB;
    Real m_fC;
};


class MAGICFM GeneralEllipse
{
public:
    // Ellipse is X^T A X + B^T X + C = 0 where A is symmetric 2x2, B and
    // X are 2x1, and C is a scalar.  Since A is symmetric, it is not
    // necessary to store all 4 values, but I use it for convenience of doing
    // matrix/vector arithmetic.
    Matrix2 m_kA;
    Vector2 m_kB;
    Real m_fC;
};


// Input:
//   eyepoint, E
//   view plane, N^t X = D
//   ellipsoid, X^t A X + B^t X + C = 0
// where E, N, B, and X are 3x1, A is symmetric 3x3, C and D are scalars
//
// Output:
//   projected ellipse, Y^t A Y + B^t Y + C = 0
// where Y and B are 2x1, A is symmetric 2x2, and C is a scalar

void PerspProjEllipsoid (const GeneralEllipsoid& rkEllipsoid,
    const Vector3& rkEye, const Plane& rkPlane, GeneralEllipse& rkEllipse);


// Compute center, axes, and axis half lengths for the ellipse
// Y^t A Y + B^t Y + C = 0.

void ConvertEllipse (GeneralEllipse& rkEllipse, Vector3& rkCenter,
    Vector3& rkAxis0, Vector3& rkAxis1, Real& rfHalfLength0,
    Real& rfHalfLength1);

} // namespace Mgc

#endif

