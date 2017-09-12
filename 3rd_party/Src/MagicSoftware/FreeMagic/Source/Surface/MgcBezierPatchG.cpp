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

#include "MgcBezierPatchG.h"
using namespace Mgc;

//----------------------------------------------------------------------------
BezierPatchG::BezierPatchG (int iDegree, int iCtrlQuantity,
    Vector3* akCtrlPoint)
{
    assert( iDegree > 0 && akCtrlPoint );

    m_iDegree = iDegree;
    m_iCtrlQuantity = iCtrlQuantity;
    m_akCtrlPoint = akCtrlPoint;

    m_iVertexQuantity = 0;
    m_akVertex = NULL;
    m_akNormal = NULL;
    m_iTriangleQuantity = 0;
    m_aiConnect = NULL;
}
//----------------------------------------------------------------------------
BezierPatchG::~BezierPatchG ()
{
    delete[] m_akCtrlPoint;
    delete[] m_akVertex;
    delete[] m_akNormal;
    delete[] m_aiConnect;
}
//----------------------------------------------------------------------------

