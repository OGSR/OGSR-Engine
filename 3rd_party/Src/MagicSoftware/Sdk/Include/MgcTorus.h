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

#ifndef MGCTORUS_H
#define MGCTORUS_H

// Implementation of torus centered at (0,0,0) with z-axis as the
// axis of symmetry (axis about which circle is rotated to generate
// the torus).
//
// algebraic form
//      Ro > 0 is radius from center of torus
//      Ri > 0 is radius of tube of torus
//      p^2 = x^2+y^2+z^2
//      p^4-2*(Ro^2+Ri^2)*p^2+4*Ro^2*z^2+(Ro^2-Ri^2)^2 = 0
//
// parametric form
//      0 <= s <= 1, 0 <= t <= 1
//      Rc = Ro+Ri*cos(2*PI*t)
//      x = Rc*cos(2*PI*s)
//      y = Rc*sin(2*PI*s)
//      z = Ri*sin(2*PI*t)

#include "MgcVector3.h"

namespace Mgc {

class MAGICFM Torus
{
public:
    Torus (Real fRo, Real fRi);

    const Real& Ro () const;
    const Real& Ri () const;

    Vector3 Position (Real fS, Real fT);
    Vector3 Normal (Real fS, Real fT);

    // for use in intersection testing
    void GetParameters (const Vector3& rkPos, Real& rfS, Real& rfT) const;

protected:
    Real m_fRo, m_fRi;
};

#include "MgcTorus.inl"

} // namespace Mgc

#endif



