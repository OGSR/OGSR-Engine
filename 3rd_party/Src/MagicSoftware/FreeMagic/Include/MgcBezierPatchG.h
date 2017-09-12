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

#ifndef MGCBEZIERPATCHG_H
#define MGCBEZIERPATCHG_H

#include "MgcParametricSurface.h"
#include "MgcVector3.h"

namespace Mgc {


class MAGICFM BezierPatchG : public ParametricSurface
{
public:
    // abstract base class for specific type and degree patches
    virtual ~BezierPatchG ();

    // patch data
    int GetDegree () const;
    int GetCtrlQuantity () const;
    Vector3* GetCtrlPoints ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector3* GetVertices ();
    Vector3* GetNormals ();
    int GetTriangleQuantity () const;
    const int* GetConnectivity () const;

    // tessellation by recursive subdivision
    virtual void Tessellate (int uiLevel, bool bWantNormals) = 0;

protected:
    // Construction.  MgcBezierPatchG accepts responsibility for deleting
    // the input arrays.
    BezierPatchG (int iDegree, int iCtrlQuantity, Vector3* akCtrlPoint);

    // tessellation
    virtual void Initialize (int iLevel, bool bWantNormals) = 0;

    // patch data
    int m_iDegree;
    int m_iCtrlQuantity;
    Vector3* m_akCtrlPoint;

    // tessellation data
    int m_iVertexQuantity;
    Vector3* m_akVertex;
    Vector3* m_akNormal;
    int m_iTriangleQuantity;
    int* m_aiConnect;
};

#include "MgcBezierPatchG.inl"

} // namespace Mgc

#endif

