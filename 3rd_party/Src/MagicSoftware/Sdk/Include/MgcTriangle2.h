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

#ifndef MGCTRIANGLE2_H
#define MGCTRIANGLE2_H

// Two different representations of the triangle are allowed.  If one form is
// used, the other is *not* automatically set.

#include "MagicFMLibType.h"
#include "MgcVector2.h"
#include "MgcRTLib.h"

namespace Mgc {


class MAGICFM Triangle2
{
public:
    Triangle2 ();

    // Triangle points are tri(s,t) = b+s*e0+t*e1 where 0 <= s <= 1,
    // 0 <= t <= 1, and 0 <= s+t <= 1.
    Vector2& Origin ();
    const Vector2& Origin () const;
    Vector2& Edge0 ();
    const Vector2& Edge0 () const;
    Vector2& Edge1 ();
    const Vector2& Edge1 () const;

    // Triangle points are V0, V1, V2.
    Vector2& Vertex (unsigned int uiI);
    const Vector2& Vertex (unsigned int uiI) const;

protected:
    // parametric form
    Vector2 m_kOrigin;
    Vector2 m_kEdge0;
    Vector2 m_kEdge1;

    // vertex form
    Vector2 m_akV[3];
};

#include "MgcTriangle2.inl"

} // namespace Mgc

#endif


