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

#ifndef MGCINTERP2DVECTORFIELD_H
#define MGCINTERP2DVECTORFIELD_H

// Given points (x0[i],y0[i]) which are mapped to (x1[i],y1[i]) for
// 0 <= i < N, interpolate positions (xIn,yIn) to (xOut,yOut).

#include "MgcInterp2DQdrNonuniform.h"

namespace Mgc {


class MAGICFM Interp2DVectorField
{
public:
    // Construction and destruction.  MgcInterp2DVectorField accepts ownership
    // of the input arrays and will delete them during destruction.
    Interp2DVectorField (int iQuantity, Vector2* akDomain, Vector2* akRange);

    ~Interp2DVectorField ();

    // Return 'true' if and only if (xIn,yIn) is in the convex hull of the
    // input points.  In this case, (xOut,yOut) is valid.  If the return
    // value is 'false', (xOut,yOut) is invalid and should not be used.
    bool Evaluate (const Vector2& rkInput, Vector2& rkOutput); 

protected:
    Interp2DQdrNonuniform* m_pkXInterp;
    Interp2DQdrNonuniform* m_pkYInterp;
};

} // namespace Mgc

#endif


