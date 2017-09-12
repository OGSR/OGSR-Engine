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

#ifndef MGCELLIPSOID_H
#define MGCELLIPSOID_H

#include "MgcMatrix3.h"

namespace Mgc {


class MAGICFM EllipsoidStandard
{
public:
    // (x/a)^2 + (y/b)^2 + (z/c)^2 = 1

    EllipsoidStandard ();

    Real& Extent (int i);
    const Real& Extent (int i) const;
    Real* Extents ();
    const Real* Extents () const;

protected:
    Real m_afExtent[3];
};


class MAGICFM Ellipsoid
{
public:
    // center-matrix form, (X-C)^T A (X-C) = 1, where A is a positive
    // definite matrix

    Ellipsoid ();

    Vector3& Center ();
    const Vector3& Center () const;

    Matrix3& A ();
    const Matrix3& A () const;

    Matrix3& InverseA ();
    const Matrix3& InverseA () const;

protected:
    Vector3 m_kCenter;
    Matrix3 m_kA;
    Matrix3 m_kInverseA;  // for intersection and culling support
};

#include "MgcEllipsoid.inl"

} // namespace Mgc

#endif


