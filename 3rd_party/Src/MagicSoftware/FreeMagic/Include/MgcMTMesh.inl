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
inline int MTMesh::GetVQuantity () const
{
    return m_akVertex.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTMesh::V (int iLabel) const
{
    VCIter pkV = m_kVMap.find(MTIVertex(iLabel));
    return pkV != m_kVMap.end() ? pkV->second : -1;
}
//----------------------------------------------------------------------------
inline const MTVertex& MTMesh::GetVertex (int iVIndex) const
{
    assert( 0 <= iVIndex && iVIndex < m_akVertex.GetQuantity() );
    return m_akVertex.Get(iVIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetVLabel (int iVIndex) const
{
    assert( 0 <= iVIndex && iVIndex < m_akVertex.GetQuantity() );
    return m_akVertex.Get(iVIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetEQuantity () const
{
    return m_akEdge.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTMesh::E (int iLabel0, int iLabel1) const
{
    ECIter pkE = m_kEMap.find(MTIEdge(iLabel0,iLabel1));
    return pkE != m_kEMap.end() ? pkE->second : -1;
}
//----------------------------------------------------------------------------
inline const MTEdge& MTMesh::GetEdge (int iEIndex) const
{
    assert( 0 <= iEIndex && iEIndex < m_akEdge.GetQuantity() );
    return m_akEdge.Get(iEIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetELabel (int iEIndex) const
{
    assert( 0 <= iEIndex && iEIndex < m_akEdge.GetQuantity() );
    return m_akEdge.Get(iEIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::ELabel (int iEIndex)
{
    assert( 0 <= iEIndex && iEIndex < m_akEdge.GetQuantity() );
    return m_akEdge[iEIndex].Label();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetTQuantity () const
{
    return m_akTriangle.GetQuantity();
}
//----------------------------------------------------------------------------
inline const MTTriangle& MTMesh::GetTriangle (int iTIndex) const
{
    assert( 0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity() );
    return m_akTriangle.Get(iTIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::T (int iLabel0, int iLabel1, int iLabel2) const
{
    TCIter pkT = m_kTMap.find(MTITriangle(iLabel0,iLabel1,iLabel2));
    return pkT != m_kTMap.end() ? pkT->second : -1;
}
//----------------------------------------------------------------------------
inline int MTMesh::GetTLabel (int iTIndex) const
{
    assert( 0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity() );
    return m_akTriangle.Get(iTIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::TLabel (int iTIndex)
{
    assert( 0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity() );
    return m_akTriangle[iTIndex].Label();
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialELabel ()
{
    return m_iInitialELabel;
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialTLabel ()
{
    return m_iInitialTLabel;
}
//----------------------------------------------------------------------------
