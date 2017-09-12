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
inline int MTEdge::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int& MTEdge::Label ()
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int MTEdge::GetVertex (int i) const
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Vertex (int i)
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int MTEdge::GetTriangle (int i) const
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Triangle (int i)
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
