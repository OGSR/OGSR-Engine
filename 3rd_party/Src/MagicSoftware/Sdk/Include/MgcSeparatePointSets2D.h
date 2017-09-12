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

#ifndef MGCSEPARATEPOINTSETS2D_H
#define MGCSEPARATEPOINTSETS2D_H

// Separate two point sets, if possible, by computing a line for which the
// point sets lie on opposite sides.  The algorithm computes the convex hull
// of the point sets, then uses the method of separating axes to determine if
// the two convex polygons are disjoint.  The convex hull calculation is
// O(n*log(n)).  There is a randomized linear approach that takes O(n), but
// I have not yet implemented it.
//
// The return value of the function is 'true' if and only if there is a
// separation.  If 'true', the returned line is a separating line.

#include "MgcLine2.h"

namespace Mgc {

MAGICFM bool SeparatePointSets2D (int iQuantity0, const Vector2* akVertex0,
    int iQuantity1, const Vector2* akVertex1, Line2& rkSeprLine);

} // namespace Mgc

#endif


