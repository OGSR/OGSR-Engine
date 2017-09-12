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

#ifndef MGCCONT2DBOX_H
#define MGCCONT2DBOX_H

#include "MgcBox2.h"

namespace Mgc {

MAGICFM void ContAlignedBox (int iQuantity, const Vector2* akPoint,
    Vector2& rkMin, Vector2& rkMax);

MAGICFM Box2 ContOrientedBox (int iQuantity, const Vector2* akPoint);


// This function allows for selection of vertices from a pool.  The return
// value is 'true' if and only if at least one vertex is valid.

MAGICFM bool ContOrientedBox (int iQuantity, const Vector2* akPoint,
    const bool* abValid, Box2& rkBox);

} // namespace Mgc

#endif


