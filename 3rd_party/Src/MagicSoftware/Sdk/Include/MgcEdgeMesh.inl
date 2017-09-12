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
inline EdgeMesh* EdgeMesh::Create () const
{
    return new EdgeMesh;
}
//----------------------------------------------------------------------------
inline void EdgeMesh::OnVertexInsert (int,bool,void*&)
{
}
//----------------------------------------------------------------------------
inline void EdgeMesh::OnVertexRemove (int,bool,void*)
{
}
//----------------------------------------------------------------------------
inline void EdgeMesh::OnEdgeInsert (const Edge&,bool,void*&)
{
}
//----------------------------------------------------------------------------
inline void EdgeMesh::OnEdgeRemove (const Edge&,bool,void*)
{
}
//----------------------------------------------------------------------------
inline bool EdgeMesh::Edge::operator== (const Edge& rkE) const
{
    return m_aiV[0] == rkE.m_aiV[0] && m_aiV[1] == rkE.m_aiV[1];
}
//----------------------------------------------------------------------------
inline bool EdgeMesh::Edge::operator!= (const Edge& rkE) const
{
    return !operator==(rkE);
}
//----------------------------------------------------------------------------
inline EdgeMesh::Edge::Edge (int iV0, int iV1)
{
    if ( iV0 < iV1 )
    {
        // v0 is minimum
        m_aiV[0] = iV0;
        m_aiV[1] = iV1;
    }
    else
    {
        // vi is minimum
        m_aiV[0] = iV1;
        m_aiV[1] = iV0;
    }
}
//----------------------------------------------------------------------------
inline bool EdgeMesh::Edge::operator< (const Edge& rkE) const
{
    if ( m_aiV[1] < rkE.m_aiV[1] )
        return true;

    if ( m_aiV[1] == rkE.m_aiV[1] )
        return m_aiV[0] < rkE.m_aiV[0];

    return false;
}
//----------------------------------------------------------------------------


