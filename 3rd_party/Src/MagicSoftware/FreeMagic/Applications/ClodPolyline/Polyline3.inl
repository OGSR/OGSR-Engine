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
inline int Polyline3::GetVertexQuantity () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
inline const Vector3* Polyline3::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline bool Polyline3::GetClosed () const
{
    return m_bClosed;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetEdgeQuantity () const
{
    return m_iEQuantity;
}
//----------------------------------------------------------------------------
inline const int* Polyline3::GetEdges () const
{
    return m_aiEdge;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMinLevelOfDetail () const
{
    return m_iVMin;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMaxLevelOfDetail () const
{
    return m_iVMax;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetLevelOfDetail () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------


