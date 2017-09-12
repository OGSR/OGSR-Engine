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

#ifndef MGCARC2_H
#define MGCARC2_H

#include "MagicFMLibType.h"
#include "MgcCircle2.h"

namespace Mgc {


class MAGICFM Arc2 : public Circle2
{
public:
    // The arc is defined by two points End0 and End1 on the circle so that
    // End1 is obtained from End0 by traversing counterclockwise.  The
    // application is responsible for ensuring that End0 and End1 are on the
    // circle and that they are properly ordered.

    Arc2 ();

    Vector2& End0 ();
    const Vector2& End0 () const;

    Vector2& End1 ();
    const Vector2& End1 () const;

    // Test if P is on the arc.  The application must ensure that P is on the
    // circle; that is, |P-C| = R.  This test works regardless of the angle
    // between B-C and A-C.
    bool Contains (const Vector2& rkP) const;

protected:
    Vector2 m_kEnd0, m_kEnd1;
};

#include "MgcArc2.inl"

} // namespace Mgc

#endif


