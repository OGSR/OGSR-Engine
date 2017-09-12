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

//----------------------------------------------------------------------------
inline int BezierPatchG::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
inline int BezierPatchG::GetCtrlQuantity () const
{
    return m_iCtrlQuantity;
}
//----------------------------------------------------------------------------
inline Vector3* BezierPatchG::GetCtrlPoints ()
{
    return m_akCtrlPoint;
}
//----------------------------------------------------------------------------
inline int BezierPatchG::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline Vector3* BezierPatchG::GetVertices ()
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline Vector3* BezierPatchG::GetNormals ()
{
    return m_akNormal;
}
//----------------------------------------------------------------------------
inline int BezierPatchG::GetTriangleQuantity () const
{
    return m_iTriangleQuantity;
}
//----------------------------------------------------------------------------
inline const int* BezierPatchG::GetConnectivity () const
{
    return m_aiConnect;
}
//----------------------------------------------------------------------------

