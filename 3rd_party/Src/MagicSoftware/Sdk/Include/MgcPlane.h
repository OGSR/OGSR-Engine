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

#ifndef MGCPLANE_H
#define MGCPLANE_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Plane
{
public:
    // The plane is represented as Dot(N,X) = c where N is the plane normal
    // vector, not necessarily unit length, c is the plane constant, and X is
    // any point on the plane.

    // N and c are uninitialized
    Plane ();

    // N and c are specified
    Plane (const Vector3& rkNormal, Real fConstant);

    // N is specified, c = Dot(N,P) where P is the input point
    Plane (const Vector3& rkNormal, const Vector3& rkPoint);

    // N = Cross(P1-P0,P2-P0), c = Dot(N,P0) where P0, P1, P2 are input points
    Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2);

    // member access
    Vector3& Normal ();
    const Vector3& Normal () const;
    Real& Constant ();
    const Real& Constant () const;

    // access plane P as P[0] = N.x, P[1] = N.y, P[2] = N.z, P[3] = c
    //
    // WARNING.  These member functions rely on
    // (1) Plane not having virtual functions
    // (2) the data packed in a 4*sizeof(Real) memory block
    Real& operator[] (int i) const;
    operator Real* ();

    // The "positive side" of the plane is the half space to which the plane
    // normal points.  The "negative side" is the other half space.  The flag
    // "no side" indicates the plane itself.
    enum Side
    {
        NO_SIDE,
        POSITIVE_SIDE,
        NEGATIVE_SIDE
    };

    Side WhichSide (const Vector3& rkPoint) const;

    // This is a pseudodistance.  The sign of the return value is positive if
    // the point is on the positive side of the plane, negative if the point
    // is on the negative side, and zero if the point is on the plane.  The
    // absolute value of the return value is the true distance only when the
    // plane normal is a unit length vector.
    Real DistanceTo (const Vector3& rkPoint) const;

    // If the plane is Dot(N,X) = c, arrange for the normal vector to be
    // unit length.  The new equation is Dot(N/|N|,X) = c/|N|.
    void Normalize ();

protected:
    Vector3 m_kNormal;
    Real m_fConstant;
};

#include "MgcPlane.inl"

} // namespace Mgc

#endif


