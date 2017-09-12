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

#ifndef MGCBEZIERCYLINDERG_H
#define MGCBEZIERCYLINDERG_H

#include "MgcBezierPatchG.h"

namespace Mgc {


class MAGICFM BezierCylinderG : public BezierPatchG
{
public:
    // The 'level' parameter in the tessellation controls the number of
    // subdivisions of the boundary curves.  The 'cylinder level' parameter
    // controls the number of subdivisions of the cylinder edges (default
    // value is 0).
    int& CylinderLevel ();

protected:
    // Construction.  MgcBezierCylinderG accepts responsibility for deleting
    // the input array.  The control points for the two boundary curves are
    // stored in the input array.
    BezierCylinderG (int iDegree, Vector3* akCtrlPoint);

    // tessellation
    virtual void Initialize (int iLevel, bool bWantNormals);

    int m_iCylinderLevel;
};

#include "MgcBezierCylinderG.inl"

} // namespace Mgc

#endif

