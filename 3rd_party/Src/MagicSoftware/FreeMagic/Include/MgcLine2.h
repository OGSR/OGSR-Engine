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

#ifndef MGCLINE2_H
#define MGCLINE2_H

// Two different representations of the line are allowed.  If one form is
// used, the other is *not* automatically set.

#include "MagicFMLibType.h"
#include "MgcVector2.h"

namespace Mgc {


class MAGICFM Line2
{
public:
    Line2 ();

    // parametric form:
    //   origin = V0, direction = V1-V0
    //
    // normal form:
    //   normal = Perp(V1-V0) where Perp(x,y)=(y,-x),
    //   constant = Dot(normal,V0)
    Line2 (const Vector2& rkV0, const Vector2& rkV1, bool bParametricForm);

    // Line is L(t) = P+t*D for any real-valued t.  D is not necessarily
    // unit length.
    Vector2& Origin ();
    const Vector2& Origin () const;
    Vector2& Direction ();
    const Vector2& Direction () const;

    // Line is Dot(N,X) = c.  N is not necessarily unit length.
    Vector2& Normal ();
    const Vector2& Normal () const;
    Real& Constant ();
    const Real& Constant () const;

    // Compute d = Dot(N,V)-c.  The value |d| is the true distance from V
    // to the line whenever |N| = 1.
    Real GetPseudodistance (const Vector2& rkV) const;

protected:
    // parametric form
    Vector2 m_kOrigin;
    Vector2 m_kDirection;

    // normal form
    Vector2 m_kNormal;
    Real m_fConstant;
};

#include "MgcLine2.inl"

} // namespace Mgc

#endif


