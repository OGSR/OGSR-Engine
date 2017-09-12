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

#ifndef MGCAPPR2DELLIPSEBYARCS_H
#define MGCAPPR2DELLIPSEBYARCS_H

#include "MgcVector2.h"

namespace Mgc {

// The ellipse is (x/a)^2 + (y/b)^2 = 1, but only the portion in the first
// quadrant (x >= 0 and y >= 0) is approximated.  Generate iNumArcs >= 2 arcs
// by constructing points corresponding to the weighted averages of the
// curvatures at the ellipse points (a,0) and (0,b).  The returned input point
// array has iNumArcs+1 elements and the returned input center and radius
// arrays each have iNumArc elements.  The arc associated with akPoint[i] and
// akPoint[i+1] has center akCenter[i] and radius afRadius[i].

MAGICFM void ApproximateEllipseByArcs (Real fA, Real fB, int iNumArcs,
    Vector2*& rakPoint, Vector2*& rakCenter, Real*& rafRadius);

}  // namespace Mgc

#endif


