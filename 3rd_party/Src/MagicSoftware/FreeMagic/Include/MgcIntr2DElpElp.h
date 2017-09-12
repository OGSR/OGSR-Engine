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

#ifndef MGCINTR2DELPELP_H
#define MGCINTR2DELPELP_H

#include "MgcVector2.h"

namespace Mgc {


// The quadratic equation representing the ellipse is
//   Q(x,y) = a*x^2 + b*x*y + c*y^2 + d*x + e*y + f = 0
// where b*b < 4*a*c is required for this to represent an ellipse.
class MAGICFM QuadPoly2
{
public:
    Real m_fA, m_fB, m_fC, m_fD, m_fE, m_fF;

    bool IsEllipse ();
};

// On return, riQuantity is the number of intersections (in {0,1,2,3,4}) and
// akP[0],...,akP[riQuantity-1] are the points of intersection.
MAGICFM void FindIntersection (const QuadPoly2& rkE0, const QuadPoly2& rkE1,
    int& riQuantity, Vector2 akP[4]);

// Return value is 'true' if and only if there is an intersection.  If there
// is no intersection, the ellipses are either separated by a line or one is
// properly contained in the other.
MAGICFM bool TestIntersection (const QuadPoly2& rkE0, const QuadPoly2& rkE1);

} // namespace Mgc

#endif


