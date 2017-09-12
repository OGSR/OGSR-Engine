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

#ifndef MGCBOX2_H
#define MGCBOX2_H

#include "MagicFMLibType.h"
#include "MgcVector2.h"
#include "MgcRTLib.h"

namespace Mgc {


class MAGICFM Box2
{
public:
    Box2 ();

    Vector2& Center ();
    const Vector2& Center () const;

    Vector2& Axis (int i);
    const Vector2& Axis (int i) const;
    Vector2* Axes ();
    const Vector2* Axes () const;

    Real& Extent (int i);
    const Real& Extent (int i) const;
    Real* Extents ();
    const Real* Extents () const;

    void ComputeVertices (Vector2 akVertex[4]) const;

protected:
    Vector2 m_kCenter;
    Vector2 m_akAxis[2];
    Real m_afExtent[2];
};

#include "MgcBox2.inl"

} // namespace Mgc

#endif


