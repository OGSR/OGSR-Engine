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

#ifndef MGCCONT2DMINBOX_H
#define MGCCONT2DMINBOX_H

#include "MgcBox2.h"

namespace Mgc {

// Compute minimum area oriented box containing the specified points.  The
// algorithm uses the rotating calipers method.  NOTE.  The input points must
// form a convex polygon and be in counterclockwise order.
MAGICFM Box2 MinBox (int iQuantity, const Vector2* akPoint);

// The slower method for computing the minimum area oriented box that does not
// maintain the extremal points supporting the box (like rotating calipers
// does).  The input points must also form a convex polygon, but the order may
// be counterclockwise or clockwise.
MAGICFM Box2 MinBoxOrderNSqr (int iQuantity, const Vector2* akPoint);

} // namespace

#endif


