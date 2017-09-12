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

#ifndef MGCCONE3_H
#define MGCCONE3_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Cone3
{
public:
    // An acute cone is Dot(A,X-V) = |X-V| cos(T) where V is the vertex, A
    // is the unit-length direction of the axis of the cone, and T is the
    // cone angle with 0 < T < PI/2.  The cone interior is defined by the
    // inequality Dot(A,X-V) >= |X-V| cos(T).  Since cos(T) > 0, we can avoid
    // computing square roots.  The solid cone is defined by the inequality
    // Dot(A,X-V)^2 >= Dot(X-V,X-V) cos(T)^2.

    Cone3 ();

    Vector3& Vertex ();
    const Vector3& Vertex () const;
    Vector3& Axis ();
    const Vector3& Axis () const;
    Real& CosAngle ();
    const Real& CosAngle () const;
    Real& SinAngle ();
    const Real& SinAngle () const;

protected:
    Vector3 m_kVertex;
    Vector3 m_kAxis;
    Real m_fCos, m_fSin;  // cos(T), sin(T)
};

#include "MgcCone3.inl"

} // namespace Mgc

#endif


