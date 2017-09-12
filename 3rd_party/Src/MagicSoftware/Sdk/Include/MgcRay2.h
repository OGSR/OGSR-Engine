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

#ifndef MGCRAY2_H
#define MGCRAY2_H

#include "MagicFMLibType.h"
#include "MgcVector2.h"

namespace Mgc {


class MAGICFM Ray2
{
public:
    // Ray is R(t) = P+t*D for t >= 0.  D is not necessarily unit length.
    Ray2 ();

    Vector2& Origin ();
    const Vector2& Origin () const;

    Vector2& Direction ();
    const Vector2& Direction () const;

protected:
    Vector2 m_kOrigin;  // P
    Vector2 m_kDirection;  // D
};

#include "MgcRay2.inl"

} // namespace Mgc

#endif


