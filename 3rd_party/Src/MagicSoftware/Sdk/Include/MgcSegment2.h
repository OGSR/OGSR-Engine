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

#ifndef MGCSEGMENT2_H
#define MGCSEGMENT2_H

#include "MagicFMLibType.h"
#include "MgcVector2.h"

namespace Mgc {


class MAGICFM Segment2
{
public:
    // Segment is S(t) = P+t*D for 0 <= t <= 1.  D is not necessarily unit
    // length.  The end points are P and P+D.
    Segment2 ();

    Vector2& Origin ();
    const Vector2& Origin () const;

    Vector2& Direction ();
    const Vector2& Direction () const;

protected:
    Vector2 m_kOrigin;  // P
    Vector2 m_kDirection;  // D
};

#include "MgcSegment2.inl"

} // namespace Mgc

#endif


