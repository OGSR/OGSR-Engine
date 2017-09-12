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

#ifndef MGCBEZIERTRIANGLEG_H
#define MGCBEZIERTRIANGLEG_H

#include "MgcBezierPatchG.h"

namespace Mgc {


class MAGICFM BezierTriangleG : public BezierPatchG
{
protected:
    // Construction.  BezierTriangleG accepts responsibility for deleting
    // the input arrays.
    BezierTriangleG (int iDegree, Vector3* akCtrlPoint);

    // tessellation
    virtual void Initialize (int iLevel, bool bWantNormals);
};

} // namespace Mgc

#endif

