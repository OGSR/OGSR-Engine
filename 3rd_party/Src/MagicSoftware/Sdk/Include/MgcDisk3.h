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

#ifndef MGCDISK3_H
#define MGCDISK3_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Disk3
{
public:
    // Plane containing disk is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Circle within
    // the plane is parameterized by X = C + R*(cos(A)*U + sin(A)*V) where
    // A is an angle in [0,2*pi).

    Disk3 ();

    Vector3& U ();
    const Vector3& U () const;

    Vector3& V ();
    const Vector3& V () const;

    Vector3& N ();
    const Vector3& N () const;

    Vector3& Center ();
    const Vector3& Center () const;

    Real& Radius ();
    const Real& Radius () const;

protected:
    Vector3 m_kU, m_kV, m_kN;
    Vector3 m_kCenter;
    Real m_fRadius;
};

#include "MgcDisk3.inl"

} // namespace Mgc

#endif


