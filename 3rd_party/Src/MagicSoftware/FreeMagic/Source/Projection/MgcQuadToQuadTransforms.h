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

#ifndef MGCQUADTOQUADTRANSFORMS_H
#define MGCQUADTOQUADTRANSFORMS_H

#include "MgcVector2.h"
#include "MgcMatrix2.h"

namespace Mgc {

//----------------------------------------------------------------------------
// Homogeneous mapping of quadrilateral <p00,p10,p11,p01> to square [0,1]^2.
// The quadrilateral points are ordered counterclockwise and map onto the
// corners (0,0), (1,0), (1,1), and (0,1), respectively.

class MAGICFM HmQuadToSqr
{
public:
    HmQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
        const Vector2& rkP11, const Vector2& rkP01);

    Vector2 Transform (const Vector2& rkP);

protected:
    Vector2 m_kT, m_kG, m_kD;
    Matrix2 m_kM;
};

//----------------------------------------------------------------------------
// Homogeneous mapping of square [0,1]^2 to quadrilateral <p00,p10,p11,p01>.
// The quadrilateral points are ordered counterclockwise and map onto the
// corners (0,0), (1,0), (1,1), and (0,1), respectively.

class MAGICFM HmSqrToQuad
{
public:
    HmSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
        const Vector2& rkP11, const Vector2& rkP01);

    Vector2 Transform (const Vector2& rkP);

protected:
    Vector2 m_kT, m_kG, m_kD;
    Matrix2 m_kM;
};

//----------------------------------------------------------------------------
// Bilinear mapping of quadrilateral <p00,p10,p11,p01> to square [0,1]^2.
// The quadrilateral points are ordered counterclockwise and map onto the
// corners (0,0), (1,0), (1,1), and (0,1), respectively.
//
// If p is strictly inside the quadrilateral, then
//   p = (1-t)*[(1-s)*p00+s*p10]+t*[(1-s)*p01+s*p11]
//     = p00 + s*(p10-p00) + t*(p01-p00) + s*t*(p11+p00-p01-p10)
//   (0,0) = (p00-p) + s*(p10-p00) + t*(p01-p00) + s*t*(p11+p00-p01-p10)
//         = A + s*B + t*C + s*t*D (this equation defines A, B, C, D)
//
// Define K((x1,y1),(x2,y2)) = x1*y2-x2*y1.  Note that K(U,V) = -K(V,U).
//   0 = K(A,A) + s*K(B,A) + t*K(C,A) + s*t*K(D,A)
//     = a0 + a1*s + a2*t + a3*s*t  (a0 = 0)
//   0 = K(A,C) + s*K(B,C) + t*K(C,C) + s*t*K(D,C)
//     = b0 + b1*s + b2*t + b3*s*t  (b2 = 0)
// Eliminating s from the two equations leads to the quadratic
//   0 = (a3*b2-a2*b3)*t^2+[(a3*b0-a0*b3)+(a1*b2-a2*b1)]*t+(a1*b0-a*b1)
//     = K(A,C)*(a*t^2+b*t+c)
// where
//   a = K(D,C)
//   b = K(D,A) + K(B,C)
//   c = K(B,A)
// Since p is strictly inside, K(A,C) is not zero, so we need only solve the
// quadratic equation a*t^2+b*t+c = 0.  If there is a root t in [0,1], then
// the corresponding s is computed from 0 = A.x + s*B.x + t*C.x + s*t*D.x, so
// s = -(A.x + t*C.x)/(B.x + t*D.x).

class MAGICFM BiQuadToSqr
{
public:
    BiQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
        const Vector2& rkP11, const Vector2& rkP01);

    Vector2 Transform (const Vector2& rkP);

protected:
    Vector2 m_kA, m_kB, m_kC, m_kD;
    Real m_fBCdet, m_fCDdet;
};

//----------------------------------------------------------------------------
// Bilinear mapping of square [0,1]^2 to quadrilateral <p00,p10,p11,p01>.
// The quadrilateral points are ordered counterclockwise and map onto the
// corners (0,0), (1,0), (1,1), and (0,1), respectively.
//
// Let be in the square.  The corresponding quadrilateral point is
// p = (1-t)*[(1-s)*p00+s*p10]+t*[(1-s)*p01+s*p11].

class MAGICFM BiSqrToQuad
{
public:
    BiSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
        const Vector2& rkP11, const Vector2& rkP01);

    Vector2 Transform (const Vector2& rkP);

protected:
    Vector2 m_kS00, m_kS01, m_kS10, m_kS11;
};

} // namespace Mgc

#endif

