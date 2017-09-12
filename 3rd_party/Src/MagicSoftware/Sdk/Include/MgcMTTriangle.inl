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
inline int MTTriangle::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Label ()
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetVertex (int i) const
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Vertex (int i)
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetEdge (int i) const
{
    return m_aiEdge[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Edge (int i)
{
    return m_aiEdge[i];
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetAdjacent (int i) const
{
    return m_aiAdjacent[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Adjacent (int i)
{
    return m_aiAdjacent[i];
}
//----------------------------------------------------------------------------
